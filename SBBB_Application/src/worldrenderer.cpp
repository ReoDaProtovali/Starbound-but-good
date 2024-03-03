#include "WorldRenderer.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include <set>
void WorldRenderer::tidy()
{
}

WorldRenderer::WorldRenderer()
{

	m_chunkFramePrev = glm::ivec4(0);
	m_tileCam.pos = glm::vec3(-1.f, -1.f, 32.0f);
	m_tileCam.tileScale = 40.0f;
	m_tileCam.disableAutoFrame();

	m_tileFBO.setColorAttachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
	m_tileFBO.addColorAttachment();
	m_tileFBO.setDimensions(glm::vec2(CHUNKSIZE * 8, CHUNKSIZE * 8));
	Texture fboTex = m_tileFBO.getColorTex(0);
	fboTex.setFiltering(GL_NEAREST, GL_NEAREST);
	m_tileSprite.attachTexture(fboTex);

	Texture& infoTex = m_tileFBO.getColorTexRef(1);
	fboTex.setFiltering(GL_NEAREST, GL_NEAREST);

	lighting.setTileInfoTex(infoTex);

	auto& res = ResourceManager::Get();
	res.loadAllTileSets();
	m_tileSheetTexture = res.getTileSheetTexture();

	glm::mat4 tmp = glm::mat4(1.f);
	m_tileShader.addMat4Uniform("transform", tmp);

	tileSheetUniformLoc = m_tileShader.addTexUniform("tileSheet", 0);

	m_tileDrawStates.attachTexture(m_tileSheetTexture);

	m_tileDrawStates.attachShader(&m_tileShader);
	lighting.setDims(16, 16);


}

int WorldRenderer::drawChunk(ChunkPos pos, uint32_t p_windowWidth)
{
	int drawnChunkCount = 0;

	if (s_chunkMap.contains(pos)) {
		if (!s_chunkMap[pos].drawable) return 0;

		m_tileDrawStates.attachShader(&m_tileShader);
		m_tileShader.use();
		m_tileShader.setTexUniform(tileSheetUniformLoc, 0);

		WorldChunk& chunk = s_chunkMap[pos];
		if (chunk.isEmpty) {
			return 0;
		};

		if (!chunk.vboIsPushed) {
			chunk.pushVBO();
		}

		// has to be shifted one chunk down on the y for reasons
		auto pixelCoords = m_tileCam.tileToPixelCoordinates(pos.x * CHUNKSIZE, pos.y * float(CHUNKSIZE) - float(CHUNKSIZE));
		GLfloat clearCol[] = { 0.f, 0.f, 0.f, 0.f };
		m_tileFBO.clearRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile), clearCol);
		m_tileFBO.clearRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile), clearCol, 1);
		m_tileFBO.clearDepthRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile));

		chunk.draw(m_tileFBO, m_tileDrawStates);

		drawnChunkCount++;
	}
	return drawnChunkCount;
}

void WorldRenderer::setCamera(Camera* p_cam)
{
	m_viewCam = p_cam;
}

int WorldRenderer::draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth)
{

	int drawnChunkCount = 0;

	if (!m_viewCam) return 0;

	auto f = m_viewCam->getFrame();
	// magic numbers
	f.y -= CHUNKSIZE * 3.f;
	f.w += CHUNKSIZE;
	f.x -= CHUNKSIZE * 2.f;
	f.z += CHUNKSIZE;

	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f);


	m_tileFBO.bind();
	m_tileFBO.setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));

	// prevent overdrawing
	static std::set<ChunkPos> seenPositions;
	static std::set<ChunkPos> seenVBOPositions;
	ChunkPos pos;
	while (auto update = m_chunkUpdateObserver.observe()) {
		if (update.value().type == ChunkUpdateType::DONE_GENERATING) {
			pos = { update.value().x, update.value().y };
			seenPositions.insert(pos);
		}
		else if (update.value().type == ChunkUpdateType::NEW_VBO_DATA) {
			pos = { update.value().x, update.value().y };
			seenVBOPositions.insert(pos);
		}
	}
	//for (ChunkPos pos : seenVBOPositions) {
	//	WorldChunk& c = s_chunkMap[pos];
	//	//c.subSingleTileVBOS();
	//	if (!c.vboIsPushed) c.pushVBO();
	//}
	seenPositions.insert(seenVBOPositions.begin(), seenVBOPositions.end());
	for (ChunkPos pos : seenPositions) {
		drawnChunkCount += drawChunk(pos, p_windowWidth);
	}
	seenPositions.clear();
	seenVBOPositions.clear();
	p_surface.bind(); // renderer fbo

	m_tileSprite.setOrigin(glm::vec2(0.f));

	m_tileSprite.draw(p_surface, p_states);

	lighting.updateInfoFBO(m_tileFBO.getColorTexRef(1), m_tileDrawStates);
	m_tileFBO.bind();

	if (chunkFrame == m_chunkFramePrev) return 0;

	pixelsPerTile = std::fminf((float)p_windowWidth / (float)m_viewCam->tileScale, 8.f);

	currentTileFrame = utils::frameToChunkCoords(f) * CHUNKSIZE;
	// breaks the frame, must be set manually
	m_tileCam.setDimensions(int(pixelsPerTile * float(currentTileFrame.z - currentTileFrame.x)), int(pixelsPerTile * float(currentTileFrame.w - currentTileFrame.y)));
	m_tileCam.setFrame(
		(float)currentTileFrame.x, (float)currentTileFrame.y, float(currentTileFrame.z - currentTileFrame.x), float(currentTileFrame.w - currentTileFrame.y)
	);
	m_tileDrawStates.setTransform(m_tileCam.getTransform());

	m_tileSprite.setBounds(Rect(0, 0, m_tileCam.getFrameDimensions().x, m_tileCam.getFrameDimensions().y));
	m_tileSprite.setPosition(glm::vec3(m_tileCam.getFrame().x, m_tileCam.getFrame().w, 0));

	lighting.updateDynamicLightingSprite(m_tileCam);

	m_tileFBO.setDimensions(glm::vec2(pixelsPerTile * (currentTileFrame.z - currentTileFrame.x), pixelsPerTile * (currentTileFrame.w - currentTileFrame.y)));
	lighting.setDims((uint16_t)m_tileFBO.getViewportWidth(), (uint16_t)m_tileFBO.getViewportHeight()); // update lighting buffer sizes

	m_tileFBO.clear();

	m_tileDrawStates.attachShader(&m_tileShader);
	m_tileShader.use();
	m_tileShader.setTexUniform(tileSheetUniformLoc, 0);

	m_tileDrawStates.m_blendMode.disable();
	drawnChunkCount += redrawCameraView(chunkFrame);

	lighting.updateInfoFBO(m_tileFBO.getColorTexRef(1), m_tileDrawStates);
	m_tileSprite.setOrigin(glm::vec2(0));

	m_chunkFramePrev = chunkFrame;
	return drawnChunkCount;
}

int WorldRenderer::redrawCameraView(const glm::vec4& chunkFrame)
{
	m_tileFBO.setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	int drawnChunkCount = 0;
	for (int y = (int)chunkFrame.y; y <= (int)chunkFrame.w; y++) {
		for (int x = (int)chunkFrame.z; x >= (int)chunkFrame.x; x--) {
			glEnable(GL_DEPTH_TEST);
			if (s_chunkMap.contains(ChunkPos(x, y))) {

				WorldChunk& chunk = s_chunkMap[ChunkPos(x, y)];
				if (!chunk.drawable) {
					continue;
				}
				if (chunk.isEmpty) {
					continue;
				};
				if (!chunk.vboIsPushed && !chunk.vboBusy)
					chunk.pushVBO();

				m_tileDrawStates.attachShader(&m_tileShader);
				m_tileShader.use();
				m_tileFBO.bind();

				drawFence = 0;
				chunk.draw(m_tileFBO, m_tileDrawStates);
				glDisable(GL_DEPTH_TEST);
				if (drawChunkBorders) {
					SBBBDebugDraw::drawBoxImmediate(chunk.getPosition().x, chunk.getPosition().y, CHUNKSIZE, CHUNKSIZE, glm::vec3(0.f, 0.f, 1.f), m_tileFBO, m_tileCam);
				}
				drawnChunkCount++;
			}
		}
	}
	return drawnChunkCount;
}