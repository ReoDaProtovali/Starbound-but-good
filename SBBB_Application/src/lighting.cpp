#include "Lighting.hpp"
#include "GameConstants.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include <stack>

Lighting::Lighting() :
	m_lightingCombineShader("./src/Shaders/LightingCombineVS.glsl", "./src/Shaders/LightingCombineFS.glsl"),
	m_dynamicLightingShader("./src/Shaders/LightingVS.glsl", "./src/Shaders/LightingFS.glsl"),
	m_ambientLightingShader("./src/Shaders/LightingVS.glsl", "./src/Shaders/AmbientLightingFS.glsl")
{
	dynamicLightingFBO.setDimensions(glm::vec2(64, 36));
	dynamicLightingFBO.getColorTexRef(0).setFiltering(GL_LINEAR, GL_LINEAR);

	ambientTex.changeDimensions(1, 1);
	ambientTex.setFiltering(GL_LINEAR, GL_LINEAR);

	lightingInfoFBO.setClearColor(glm::vec4(0.f));
	m_lightingTextureUniformLoc = m_lightingCombineShader.addTexUniform("lightingTexture", 0);
	m_combine_ambientTextureUniformLoc = m_lightingCombineShader.addTexUniform("ambientTexture", 1);
	m_screenTextureUniformLoc = m_lightingCombineShader.addTexUniform("screenTexture", 2);
	m_lightingResUniformLoc = m_lightingCombineShader.addVec2Uniform("lightingRes", glm::vec2(1.f));

	glm::mat4 tmpmat4 = glm::mat4(1.f);
	m_dynamicLightingShader.addMat4Uniform("transform", tmpmat4);
	m_ambientLightingShader.addMat4Uniform("transform", tmpmat4);
	m_lightingInfoTexUniformLoc = m_dynamicLightingShader.addTexUniform("lightingInfoTex", 0);

	m_ambient_ambientTextureUniformLoc = m_ambientLightingShader.addTexUniform("ambientLightingTex", 0);

	dynamicLightingSprite.attachShader(&m_dynamicLightingShader);
	ambientSprite.attachShader(&m_ambientLightingShader);
	ambientSprite.attachTexture(ambientTex);

	// needed
	dynamicLightingSprite.setBounds({ 0, 0, 10, 10 });
	ambientSprite.setBounds({ 0, 0, 10, 10 });
	ambientSprite.setPosition({ 1.f, 1.f, 1.f });
	ambientSprite.setOriginRelative(OriginLoc::TOP_LEFT);
	ambientSprite.setScale({ 1.f, -1.f });


	// There needs to be two textures set for lighting to draw properly, but we only have one for now
	Texture tmp;
	m_lightingCombineStates.addTexture(tmp);
	m_lightingCombineStates.addTexture(tmp);
	m_lightingCombineStates.attachShader(&m_lightingCombineShader);
	//m_lightingStates.addTexture(tmp)

	ambientLightingFBO.useDepth(false);
	ambientLightingFBO.setDimensions(128, 64);

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
	ambientLightingFBO.setDimensions(p_width, p_height);
	dynamicLightingFBO.setDimensions(uint32_t(p_width / m_dynamicResDivisor), uint32_t(p_height / m_dynamicResDivisor));

}
void Lighting::processAmbientQuadrant(uint32_t sx, uint32_t sy, uint32_t dimx, uint32_t dimy, std::shared_ptr<Pixmap> infoMap)
{
	const int32_t kernelSize = 22; // might make it work differently later, the actual width is this + 1 because 0 is included

	for (uint32_t y = sy; y < sy + dimy; y++) {
		for (uint32_t x = sx; x < sx + dimx; x++) {
			glm::vec4 infoPx = infoMap->getPixel(x, y);
			if (infoPx.a == 0.f) { // ambient lights are denoted by 100% alpha pixels
				if (infoPx.x == 0.f && infoPx.y == 0.f && infoPx.z == 0) {
					infoPx = { 0.25f, 0.25f, 0.29f, 0.f };
				}
				// check if surrounded by other ambient lights, if so, skip
				bool surrounded = infoMap->getPixel(x, y + 1).a == 0.f &&
					infoMap->getPixel(x - 1, y).a == 0.f &&
					infoMap->getPixel(x, y - 1).a == 0.f &&
					infoMap->getPixel(x + 1, y).a == 0.f;
				if (surrounded) {
					ambientMap.setPixel(x, y, utils::vec4Max(glm::vec4(infoPx.r, infoPx.g, infoPx.b, 1.f), ambientMap.getPixel(x, y)));
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
					float absorbance = 1.f - std::pow(1.f - infoCanvas.getPixel(nx, ny).a, 1.5);
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


void Lighting::calculateAmbient(glm::ivec4 tileFrame, float p_pixelsPerTile)
{
	static Observer<ChunkUpdate> tileUpdates(globals.chunkUpdateSubject);
	static glm::ivec4 lastFrame = tileFrame;

	if (p_pixelsPerTile < 8.f) return;
	// tons of guards 
	static bool ambientTexUpdated = true;
	if (ambientThreadPool.waitingCount != 4) {
		return;
	}
	else if (!ambientTexUpdated) { // this executes once the texture is ready to display
		ambientTexUpdated = true;
		std::unique_lock<std::shared_mutex> lock(ambientAccessMut);
		updateAmbientLightingSprite();
		ambientTex.fromVec4Data((uint32_t)std::abs(ambientSprite.bounds.wh.x), (uint32_t)std::abs(ambientSprite.bounds.wh.y), ambientMap.getData()); // keep it up to date
		return;

	}
	bool needsRecalc = false;


	if (lastFrame != tileFrame) {
		needsRecalc = true;
		lastFrame = tileFrame;
	}

	if (tileUpdates.observe()) needsRecalc = true;

	if (!needsRecalc) return;
	m_nextAmbientTileFrame = tileFrame;
	m_nextAmbientBounds = Rect(0, 0, std::abs(m_nextAmbientTileFrame.z - m_nextAmbientTileFrame.x), std::abs(m_nextAmbientTileFrame.y - m_nextAmbientTileFrame.w));
	ambientTexUpdated = false;
	{
		std::unique_lock<std::shared_mutex> lock(ambientAccessMut);
		ambientMap.resize((uint32_t)std::abs(m_nextAmbientBounds.wh.x), (uint32_t)std::abs(m_nextAmbientBounds.wh.y));
		ambientMap.clear();
	}
	std::shared_ptr<Pixmap> infoMap = std::make_shared<Pixmap>((uint32_t)std::abs(m_nextAmbientBounds.wh.x), (uint32_t)std::abs(m_nextAmbientBounds.wh.y));
	glm::vec4* readDat = (glm::vec4*)malloc(std::abs(m_nextAmbientBounds.wh.x) * std::abs(m_nextAmbientBounds.wh.y) * sizeof(glm::vec4));

	lightingInfoFBO.bind();
	glReadPixels(0, 0, std::abs(m_nextAmbientBounds.wh.x), std::abs(m_nextAmbientBounds.wh.y), GL_RGBA, GL_FLOAT, static_cast<void*>(readDat));
	infoMap->setData(readDat);

	free(readDat);

	glm::uvec2 dims = glm::uvec2(std::abs(m_nextAmbientBounds.wh.x), std::abs(m_nextAmbientBounds.wh.y));
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, 0, 0, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, dims.x / 2, 0, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, 0, dims.y / 2, dims.x / 2, dims.y / 2, infoMap);
	ambientThreadPool.assign(&Lighting::processAmbientQuadrant, this, dims.x / 2, dims.y / 2, dims.x / 2, dims.y / 2, infoMap);
}

void Lighting::calculateDynamic(FrameBuffer& p_screenFBO)
{
	m_lights.setAt(0, { {0.5f, 70.5f}, dynamicUniforms.testLightInfo, SDL_GetTicks() / 1000.f, 0.4f });
	m_lights.setAt(1, { {-5.5f, 70.5f}, 1.f - dynamicUniforms.testLightInfo, SDL_GetTicks() / 1000.f + 3.1415f, 0.4f });
	dynamicUniforms.lightCount = 2;
	dynamicUniformBlock.setData(&dynamicUniforms);

	dynamicLightingFBO.setClearColor(glm::vec4(0));
	dynamicLightingFBO.clear();

}

void Lighting::updateDynamicLightingSprite(const Camera& p_tileCam)
{
	dynamicLightingSprite.setOrigin(glm::vec2(0.f));
	dynamicLightingSprite.setBounds(Rect(0, 0, p_tileCam.getFrameDimensions().x, p_tileCam.getFrameDimensions().y));
	dynamicLightingSprite.setPosition(glm::vec3(p_tileCam.getFrame().x, p_tileCam.getFrame().w, 0));
	dynamicUniforms.topLeftTileCoord = { dynamicLightingSprite.getPosition().x, dynamicLightingSprite.getPosition().y };
	dynamicUniforms.tileDims = { std::abs(dynamicLightingSprite.bounds.wh.x), std::abs(dynamicLightingSprite.bounds.wh.y) };
}

void Lighting::updateAmbientLightingSprite()
{
	ambientSprite.setBounds(m_nextAmbientBounds);
	ambientSprite.setOriginRelative(OriginLoc::BOTTOM_LEFT);
	ambientSprite.setPosition(glm::vec3(m_nextAmbientTileFrame.x, m_nextAmbientTileFrame.w, 1.f));
	ambientUniforms.tileDims = ambientSprite.bounds.wh;
	ambientUniforms.topLeftTileCoord = ambientSprite.getPosition();
	ambientUniformBlock.setData(&ambientUniforms);
}

void Lighting::updateInfoFBO(const Texture& p_infoTex, DrawStates& p_tileStates)
{
	lightingInfoFBO.setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	lightingInfoFBO.bind();
	lightingInfoFBO.clear();
	dynamicLightingSprite.attachTexture(p_infoTex);
	p_tileStates.m_blendMode.disable();
	dynamicLightingSprite.attachShader(&gs.imageShader); // temporarily hijacking this sprite to update the info FBO
	dynamicLightingSprite.draw(lightingInfoFBO, p_tileStates);
}

void Lighting::draw(FrameBuffer& p_screenFBO, DrawSurface& p_gameWindow, DrawStates& p_states, glm::ivec4 p_currentTileFrame, float p_pixelsPerTile)
{
	DrawStates newStates(p_states);
	newStates.m_blendMode.enable();
	glDisable(GL_DEPTH_TEST);
	if (!m_screenTexSet) {

		m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
		m_lightingCombineStates.setTexture(1, ambientLightingFBO.getColorTexRef(0));
		// Now that we know which FBO we are drawing to, we can set the texture.
		m_lightingCombineStates.addTexture(p_screenFBO.getColorTexRef(0));
		m_screenTexSet = true;
	}

	Texture& infoTex = lightingInfoFBO.getColorTexRef(0);

	DrawStates dynamicLightingStates(p_states);
	dynamicLightingStates.addTexture(infoTex);

	m_dynamicLightingShader.setTexUniform(m_lightingInfoTexUniformLoc, 0);

	ImGui::Begin("Lighting");
	ImGui::SliderFloat("Resolution Divisor", &m_dynamicResDivisor, 1.f, 64.f);
	static bool ambient = true;
	static bool dynamic = true;
	ImGui::Checkbox("Ambient", &ambient);
	ImGui::Checkbox("Dynamic", &dynamic);
	ImGui::ColorPicker4("Light Color", glm::value_ptr(dynamicUniforms.testLightInfo));
	ImGui::End();

	if (ambient)
		calculateAmbient(p_currentTileFrame, p_pixelsPerTile);
	if (dynamic)
		calculateDynamic(p_screenFBO);

	if (dynamic) {
		m_dynamicLightingShader.use();
		dynamicLightingSprite.attachShader(&m_dynamicLightingShader);
		dynamicLightingFBO.bind();
		dynamicLightingSprite.attachTexture(infoTex);
		dynamicLightingSprite.draw(dynamicLightingFBO, dynamicLightingStates);
	}
	else if (ambient) {
		dynamicLightingFBO.setClearColor({ 0.0f, 0.0f, 0.0f, 1.f });
		dynamicLightingFBO.clear();
	}

	if (ambient) {
		m_ambientLightingShader.use();
		ambientLightingFBO.bind();
		ambientLightingFBO.setClearColor({ 0.f, 0.f, 0.f, 0.f });
		ambientLightingFBO.clear();
		ambientSprite.attachTexture(ambientTex);
		ambientSprite.draw(ambientLightingFBO, p_states);
	}
	else if (dynamic) {
		ambientLightingFBO.setClearColor({ 0.0f, 0.0f, 0.0f, 1.f });
		ambientLightingFBO.clear();
	}

	if (!ambient && !dynamic) {
		ambientLightingFBO.setClearColor({ 0.25f, 0.25f, 0.25f, 1.f });
		ambientLightingFBO.clear();
		dynamicLightingFBO.setClearColor({ 0.25f, 0.25f, 0.25f, 1.f });
		dynamicLightingFBO.clear();
	}
	//m_lightingCombineStates.setTexture(0, ambientTex);
	//m_lightingCombineStates.setTexture(0, dynamicLightingFBO.getColorTexRef(0));
	//m_lightingCombineStates.setTexture(1, ambientLightingFBO.getColorTexRef(0));
	m_lightingCombineShader.setVec2Uniform(m_lightingResUniformLoc, glm::vec2(dynamicLightingFBO.getViewportWidth(), dynamicLightingFBO.getViewportHeight()));
	p_gameWindow.bind();
	// something I realize now is that calling .draw on an FBO does not actually bind the FBO, it just draws to the current bound thing.
	p_gameWindow.draw(m_overlayMesh, GL_TRIANGLES, m_lightingCombineStates);
}