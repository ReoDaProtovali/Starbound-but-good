#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include <stack>

Lighting::Lighting() :
	m_framePos(0.f, 0.f),
	m_frameDim(0.f, 0.f),
	m_lightingCombineShader("./src/Shaders/LightingCombineVS.glsl", "./src/Shaders/LightingCombineFS.glsl"),
	m_dynamicLightingShader("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl")
{
	dynamicLightingFBO.setDimensions(glm::vec2(64, 36));
	dynamicLightingFBO.getColorTexRef(0).setFiltering(GL_LINEAR, GL_LINEAR);

	ambientTex.changeDimensions(1, 1);
	ambientTex.setFiltering(GL_LINEAR, GL_LINEAR);

	lightingInfoFBO.setClearColor(glm::vec4(0.f));
	m_lightingTextureUniformLoc = m_lightingCombineShader.addTexUniform("lightingTexture", 0);
	m_screenTextureUniformLoc = m_lightingCombineShader.addTexUniform("screenTexture", 1);
	m_lightingResUniformLoc = m_lightingCombineShader.addVec2Uniform("lightingRes", glm::vec2(1.f));

	glm::mat4 tmpmat4 = glm::mat4(1.f);
	m_dynamicLightingShader.addMat4Uniform("transform", tmpmat4);
	m_lightingInfoTexUniformLoc = m_dynamicLightingShader.addTexUniform("lightingInfoTex", 0);
	m_ambientLightingTextureUniformLoc = m_dynamicLightingShader.addTexUniform("ambientLightingTex", 1);

	lightingSprite.attachShader(&m_dynamicLightingShader);

	// There needs to be two textures set for lighting to draw properly, but we only have one for now
	Texture tmp;
	m_lightingCombineStates.addTexture(tmp);
	m_lightingCombineStates.attachShader(&m_lightingCombineShader);
	//m_lightingStates.addTexture(tmp)
	lightingInfoFBO.useDepth(false);
	lightingInfoFBO.setDimensions(128, 64);
	lightingInfoFBO.getColorTexRef(0).setFiltering(GL_NEAREST, GL_NEAREST);
	lightingInfoFBO.getColorTexRef(0).setWrapping(GL_CLAMP_TO_EDGE);
	m_overlayMesh.addFloatAttrib(3); // Position attrib
	m_overlayMesh.addFloatAttrib(2); // Tex Coord attrib

	dynamicLightingFBO.getColorTexRef(0).setWrapping(GL_CLAMP_TO_EDGE);

	m_overlayMesh.pushVertices({ // pushing the whole quad vertex array in one swoop
	-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // vertex 1
	1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 2
	-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 3
	-1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // vertex 4
	1.0f, -1.0f, 1.0f,  1.0f, 0.0f, // vertex 5
	1.0f,  1.0f, 1.0f,  1.0f, 1.0f // vertex 6
		});

	m_overlayMesh.pushVBOToGPU();

}


Lighting::~Lighting()
{

}

void Lighting::setTileInfoTex(Texture p_infoTex)
{
	m_lightingInfoTex = p_infoTex;
}

void Lighting::setDims(uint16_t p_width, uint16_t p_height)
{
	if (p_width < 9 || p_height < 9) {
		ERROR_LOG("Invalid Lighting Dims");
		return;
	}
	lightingInfoFBO.setDimensions(p_width / 8, p_height / 8);

}
void Lighting::processAmbientQuadrant(uint32_t sx, uint32_t sy, uint32_t dimx, uint32_t dimy, std::shared_ptr<Pixmap> infoMap)
{
	const int32_t kernelSize = 22; // might make it work differently later, the actual width is this + 1 because 0 is included

	for (uint32_t y = sy; y < sy + dimy; y++) {
		for (uint32_t x = sx; x < sx + dimx; x++) {
			glm::vec4 infoPx = infoMap->getPixel(x, y);
			if (infoPx.a == 0.f) { // ambient lights are denoted by 100% alpha pixels
				if (infoPx.x == 0.f && infoPx.y == 0.f && infoPx.z == 0) {
					infoPx = { 0.25f, 0.25f, 0.25f, 0.f };
				}
				// check if surrounded by other ambient lights, if so, skip
				bool surrounded = infoMap->getPixel(x, y + 1).a == 0.f &&
					infoMap->getPixel(x - 1, y).a == 0.f &&
					infoMap->getPixel(x, y - 1).a == 0.f &&
					infoMap->getPixel(x + 1, y).a == 0.f;
				if (surrounded) {
					ambientMap.setPixel(x, y, glm::vec4(infoPx.r, infoPx.g, infoPx.b, 1.f));
					continue;
				}
				// reaching this section means that there's an ambient light that matters here
				Pixmap infoCanvas{ kernelSize + 1, kernelSize + 1 }; // a little bit of caching
				Pixmap lightCanvas{ kernelSize + 1, kernelSize + 1 };

				for (int32_t ky = -kernelSize / 2; ky <= kernelSize / 2; ky++) {
					for (int32_t kx = -kernelSize / 2; kx <= kernelSize / 2; kx++) {
						infoCanvas.setPixel(kx + kernelSize / 2, ky + kernelSize / 2, infoMap->getPixel(x + kx, y + ky));
					}
				}
				lightCanvas.fill(glm::vec4(0.f, 0.f, 0.f, 1.f));
				lightCanvas.setPixel(kernelSize / 2, kernelSize / 2, { infoPx.r, infoPx.g, infoPx.b, 1.f });

				floodAmbient(x, y, lightCanvas, infoCanvas);

			}
		}
	}
}
// im just separating this so I can thread it
void Lighting::floodAmbient(uint32_t lx, uint32_t ly, Pixmap& lightCanvas, Pixmap& infoCanvas)
{
	std::stack<glm::ivec2> lightStack{};
	const glm::ivec2 center = { lightCanvas.width / 2, lightCanvas.height / 2 };
	const int maxLightDistance = lightCanvas.width / 2;
	lightStack.push(center);

	while (!lightStack.empty()) {
		glm::ivec2 pos = lightStack.top();
		lightStack.pop();

		int layer = std::max(std::abs(pos.x - center.x), std::abs(pos.y - center.y));

		glm::vec4 targetColor = lightCanvas.getPixel(pos.x, pos.y);

		bool transmitFurther = false;

		const float LIGHT_CUTOFF = 0.01f;
		if ((targetColor.r > LIGHT_CUTOFF ||
			targetColor.g > LIGHT_CUTOFF ||
			targetColor.b > LIGHT_CUTOFF)
			) {
			transmitFurther = true;
		}

		if (pos != center && !transmitFurther) { continue; }

		for (int ny = pos.y - 1; ny <= pos.y + 1; ++ny) {
			for (int nx = pos.x - 1; nx <= pos.x + 1; ++nx) {
				if (nx == pos.x && ny == pos.y) continue;
				int neighborLayer = std::max(std::abs(nx - center.x), std::abs(ny - center.y));
				if (neighborLayer < maxLightDistance && neighborLayer == layer + 1) {
					float absorbance = infoCanvas.getPixel(nx, ny).a;
					float centerDist = 1.f - std::powf(std::sqrtf(std::pow(nx - center.x, 2) + std::pow(ny - center.y, 2)) / (maxLightDistance), 2.f);

					if ((nx != pos.x) && (ny != pos.y)) {
						absorbance = std::pow(absorbance, 1.41421f);
					}
					int emitX = nx;
					int emitY = ny;
					glm::vec4 newCol = lightCanvas.getPixel(emitX, emitY);

					if (newCol.r + newCol.g + newCol.b == 0.f) {
						lightStack.push(glm::ivec2(nx, ny));
					}

					lightCanvas.setPixel(emitX, emitY, centerDist * absorbance * targetColor);

				}
			}
		}
	}

	{
		int kernelSize = (int)lightCanvas.height;
		std::unique_lock lock{ambientAccessMut};
		for (int32_t ky = -kernelSize / 2; ky <= kernelSize / 2; ky++) {
			for (int32_t kx = -kernelSize / 2; kx <= kernelSize / 2; kx++) {
				ambientMap.setPixel(
					lx + kx,
					ly + ky,
					utils::vec4Max(
						lightCanvas.getPixel(kx + lightCanvas.width / 2, ky + lightCanvas.height / 2),
						ambientMap.getPixel(lx + kx, ly + ky)
					));
			}
		}
	}
}


void Lighting::calculateAmbient()
{
	static Observer<ChunkUpdate> tileUpdates(globals.chunkUpdateSubject);
	static glm::uvec2 dims{(uint32_t)std::abs(lightingSprite.bounds.wh.x), (uint32_t)std::abs(lightingSprite.bounds.wh.y)};

	glm::uvec2 newDims = glm::uvec2((uint32_t)std::abs(lightingSprite.bounds.wh.x), (uint32_t)std::abs(lightingSprite.bounds.wh.y));

	// tons of guards 
	static bool ambientTexUpdated = true;
	if (ambientThreadPool.waitingCount != 4) {
		return;
	}
	else if (!ambientTexUpdated) {
		ambientTexUpdated = true;
		std::unique_lock<std::shared_mutex> lock(ambientAccessMut);
		ambientTex.fromVec4Data(dims.x, dims.y, ambientMap.getData()); // keep it up to date
	}
	bool needsRecalc = false;

	if (dims != newDims) {
		needsRecalc = true;
		dims = newDims;
	}

	if (tileUpdates.observe()) needsRecalc = true;
	tileUpdates.clear();

	if (!needsRecalc) return;
	ambientTexUpdated = false;
	{
		std::unique_lock<std::shared_mutex> lock(ambientAccessMut);
		ambientMap.resize(dims.x, dims.y);
		ambientMap.clear();
	}
	std::shared_ptr<Pixmap> infoMap = std::make_shared<Pixmap>(dims.x, dims.y);
	glm::vec4* readDat = (glm::vec4*)malloc(dims.x * dims.y * sizeof(glm::vec4));

	lightingInfoFBO.bind();
	glReadPixels(0, 0, std::abs(lightingSprite.bounds.wh.x), std::abs(lightingSprite.bounds.wh.y), GL_RGBA, GL_FLOAT, static_cast<void*>(readDat));
	infoMap->setData(readDat);

	free(readDat);

	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, 0, 0, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, dims.x / 2, 0, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, 0, dims.y / 2, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, dims.x / 2, dims.y / 2, dims.x / 2, dims.y / 2, infoMap);

	//processAmbientQuadrant(0, 0,                   dims.x / 2, dims.y / 2, infoMap);
	//processAmbientQuadrant(dims.x / 2, 0,          dims.x / 2, dims.y / 2, infoMap);
	//processAmbientQuadrant(0, dims.y / 2,          dims.x / 2, dims.y / 2, infoMap);
	//processAmbientQuadrant(dims.x / 2, dims.y / 2, dims.x / 2, dims.y / 2, infoMap);
	//processAmbientQuadrant(0, 0, dims.x, dims.y, infoMap);

}

void Lighting::calculateDynamic(FrameBuffer& p_screenFBO)
{
	dynamicUniforms.topLeftTileCoord = { lightingSprite.getPosition().x, lightingSprite.getPosition().y };
	dynamicUniforms.tileDims = { std::abs(lightingSprite.bounds.wh.x), std::abs(lightingSprite.bounds.wh.y) };
	m_lights.setAt(0, { {0.5f, 70.5f}, {1.f, 0.25f, 0.25f}, SDL_GetTicks() / 1000.f, 0.4f });
	m_lights.setAt(1, { {-5.5f, 70.5f}, {0.25f, 0.25f, 1.f}, SDL_GetTicks() / 1000.f + 3.1415f, 0.4f });
	dynamicUniforms.lightCount = 2;
	dynamicUniformBlock.setData(&dynamicUniforms);

	dynamicLightingFBO.setDimensions(uint32_t(p_screenFBO.getViewportWidth() / dynamicResDivisor), uint32_t(p_screenFBO.getViewportHeight() / dynamicResDivisor));
	dynamicLightingFBO.setClearColor(glm::vec4(0));
	dynamicLightingFBO.clear();

}

void Lighting::draw(FrameBuffer& p_screenFBO, DrawSurface& p_gameWindow, DrawStates& p_states)
{
	DrawStates newStates(p_states);
	newStates.m_blendMode.enable();
	glDisable(GL_DEPTH_TEST);
	if (!m_screenTexSet) {

		m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
		// Now that we know which FBO we are drawing to, we can set the texture.
		m_lightingCombineStates.addTexture(p_screenFBO.getColorTexRef(0));
		m_screenTexSet = true;
	}

	Texture& infoTex = lightingInfoFBO.getColorTexRef(0);

	DrawStates dynamicLightingStates(p_states);
	dynamicLightingStates.addTexture(infoTex);
	dynamicLightingStates.addTexture(ambientTex);

	m_dynamicLightingShader.setTexUniform(m_lightingInfoTexUniformLoc, 0);
	m_dynamicLightingShader.setTexUniform(m_ambientLightingTextureUniformLoc, 1);

	ImGui::Begin("Lighting");
	ImGui::SliderFloat("Resolution Divisor", &dynamicResDivisor, 1.f, 64.f);
	static bool fullbright = false;
	ImGui::Checkbox("Fullbright", &fullbright);
	ImGui::ColorPicker4("Light Color", glm::value_ptr(dynamicUniforms.testLightInfo));
	ImGui::End();

	if (!fullbright)
		calculateAmbient();

	calculateDynamic(p_screenFBO);

	m_dynamicLightingShader.use();

	lightingSprite.attachShader(&m_dynamicLightingShader);
	if (!fullbright) {
		dynamicLightingStates.setTexture(0, infoTex);
		dynamicLightingStates.setTexture(1, ambientTex);
		lightingSprite.draw(dynamicLightingFBO, dynamicLightingStates);
	}
	else {
		dynamicLightingFBO.setClearColor({ 0.25f, 0.25f, 0.25f, 1.f });
		dynamicLightingFBO.clear();
	}
	//m_lightingCombineStates.setTexture(0, ambientTex);
	m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
	m_lightingCombineShader.setVec2Uniform(m_lightingResUniformLoc, glm::vec2(dynamicLightingFBO.getViewportWidth(), dynamicLightingFBO.getViewportHeight()));
	p_gameWindow.bind();
	// something I realize now is that calling .draw on an FBO does not actually bind the FBO, it just draws to the current bound thing.
	p_gameWindow.draw(m_overlayMesh, GL_TRIANGLES, m_lightingCombineStates);
}