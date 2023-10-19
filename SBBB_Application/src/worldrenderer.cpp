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
	m_tileFBO.setDimensions(glm::vec2(10.f, 10.f));
	Texture fboTex = m_tileFBO.getColorTex(0);
	fboTex.setFiltering(GL_NEAREST, GL_NEAREST);
	m_tileSprite.attachTexture(fboTex);

	Texture& infoTex = m_tileFBO.getColorTexRef(1);
	fboTex.setFiltering(GL_NEAREST, GL_NEAREST);
	//infoTex.setChannels(GL_RGB);
	lighting.setTileInfoTex(infoTex);

	auto& res = ResourceManager::Get();
	res.loadAllTileSets();
	m_tileSheetTexture = res.getTileSheetTexture();

	glm::mat4 tmp = glm::mat4(1.f);
	m_tileShader.addMat4Uniform("transform", tmp);
	tileSheetHeightUniformLoc = m_tileShader.addIntUniform("tileSheetHeight", m_tileSheetTexture.height);
	worldPosUniformLoc = m_tileShader.addVec2Uniform("worldPos", glm::vec2(0.f));
	tileSheetUniformLoc = m_tileShader.addTexUniform("tileSheet", 0);
	drawAsBordersUniformLoc = m_tileShader.addBoolUniform("drawAsBorders", 0);

	m_tileFeedbackShader.addMat4Uniform("transform", tmp);
	feedback_tileSheetUniformLoc = m_tileFeedbackShader.addTexUniform("tileSheet", 0);
	feedback_tileSheetHeightUniformLoc = m_tileFeedbackShader.addIntUniform("tileSheetHeight", m_tileSheetTexture.height);

	m_tileDrawStates.attachTexture(m_tileSheetTexture);

	m_tileDrawStates.attachShader(&m_tileShader);
	lighting.setDims(16, 16);


}

int WorldRenderer::redrawCameraView()
{
	m_tileFBO.clear();
	m_tileFBO.setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	glEnable(GL_DEPTH_TEST);
	int drawnChunkCount = 0;
	for (int y = (int)m_chunkFramePrev.y; y <= (int)m_chunkFramePrev.w; y++) {
		for (int x = (int)m_chunkFramePrev.x; x <= (int)m_chunkFramePrev.z; x++) {
			if (s_chunkMap.contains(ChunkPos(x, y))) {

				WorldChunk& chunk = s_chunkMap[ChunkPos(x, y)];
				if (!chunk.drawable) {
					continue;
				}
				if (chunk.isEmpty) {
					continue;
				};
				if (!chunk.vboIsPushed) {
					chunk.pushVBO();
				}
				if (!chunk.tilesToSub.empty()) {
					chunk.subSingleTileVBOS();
				}
				auto pixelCoords = m_tileCam.tileToPixelCoordinates(x * (float)CHUNKSIZE, y * (float)CHUNKSIZE - (float)CHUNKSIZE);

				//if (chunk.feedbackMeshReady) {
				//	m_tileDrawStates.attachShader(&m_tileFeedbackShader);
				//	m_tileFeedbackShader.setIntUniform(feedback_tileSheetHeightUniformLoc, tilesheet.height);
				//	m_tileFeedbackShader.setTexUniform(feedback_tileSheetUniformLoc, 0);
				//	m_tileFeedbackShader.use();
				//}
				//else {
				m_tileDrawStates.attachShader(&m_tileShader);
				m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
				m_tileShader.use();
				//}
				//p_surface.bind();
				m_tileFBO.bind();
				chunk.draw(m_tileFBO, m_tileDrawStates);
				if (drawChunkBorders) {
					SBBBDebugDraw::drawBoxImmediate(chunk.getPosition().x, chunk.getPosition().y, CHUNKSIZE, CHUNKSIZE, glm::vec3(0.f, 0.f, 1.f), m_tileFBO, m_tileCam);
				}
				drawnChunkCount++;
			}
		}
	}
	return drawnChunkCount;
}

int WorldRenderer::drawChunk(ChunkPos pos, uint32_t p_windowWidth)
{
	int drawnChunkCount = 0;
	pixelsPerTile = (float)p_windowWidth / (float)m_viewCam->tileScale;
	m_tileCam.setDimensions(int(pixelsPerTile * float(currentTileFrame.z - currentTileFrame.x)), int(pixelsPerTile * float(currentTileFrame.w - currentTileFrame.y)));

	// has to be shifted one chunk down on the y for reasons
	auto pixelCoords = m_tileCam.tileToPixelCoordinates(pos.x * CHUNKSIZE - CHUNKSIZE, pos.y * float(CHUNKSIZE) - float(CHUNKSIZE * 2));
	GLfloat clearCol[] = { 0.f, 0.f, 0.f, 0.f };
	m_tileFBO.clearRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3.f, (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3.f, clearCol);
	m_tileFBO.clearRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3.f, (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3.f, clearCol, 1);
	m_tileFBO.clearDepthRegion((GLsizei)roundf(pixelCoords.x), (GLsizei)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3, (GLsizei)roundf(CHUNKSIZE * pixelsPerTile) * 3);

	if (s_chunkMap.contains(pos)) {

		WorldChunk& chunk = s_chunkMap[pos];
		if (!chunk.drawable) {
			return 0;
		}
		if (chunk.isEmpty) {
			return 0;
		};
		if (!chunk.vboIsPushed) {
			chunk.pushVBO();
		}
		if (!chunk.tilesToSub.empty()) {
			chunk.subSingleTileVBOS();
		}
		m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
		m_tileShader.setBoolUniform(drawAsBordersUniformLoc, false);
		//chunk.draw(m_tileFBO, m_tileDrawStates);
		// workaround to fix chunk border bug
		glEnable(GL_DEPTH_TEST);
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				//if (i == 0 && j == 0) continue;
				auto neighborPos = ChunkPos(pos.x + i, pos.y + j);
				if (!s_chunkMap.contains(neighborPos)) continue;
				WorldChunk& c = s_chunkMap[neighborPos];
				if (!c.drawable) continue;
				if (!c.vboIsPushed) {
					c.pushVBO();
				}
				if (!c.tilesToSub.empty()) {
					c.subSingleTileVBOS();
				}
				m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(c.worldPos.x, c.worldPos.y));
				m_tileShader.setBoolUniform(drawAsBordersUniformLoc, false);
				m_tileDrawStates.m_blendMode.disable();
				c.draw(m_tileFBO, m_tileDrawStates);
			}
		}
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				//if (i == 0 && j == 0) continue;
				auto neighborPos = ChunkPos(pos.x + i, pos.y + j);
				if (!s_chunkMap.contains(neighborPos)) continue;
				WorldChunk& c = s_chunkMap[neighborPos];
				if (!c.drawable) continue;
				m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(c.worldPos.x, c.worldPos.y));
				m_tileShader.setBoolUniform(drawAsBordersUniformLoc, true);
				m_tileDrawStates.m_blendMode.enable();
				glEnable(GL_DEPTH_TEST);
				c.draw(m_tileFBO, m_tileDrawStates);
				glDisable(GL_DEPTH_TEST);
				if (drawChunkBorders) {
					SBBBDebugDraw::drawBoxImmediate(c.getPosition().x, c.getPosition().y, CHUNKSIZE, CHUNKSIZE, glm::vec3(0.f, 0.f, 1.f), m_tileFBO, m_tileCam);
				}
			}
		}

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
	if (!m_viewCam) return 0;

	auto f = m_viewCam->getFrame();
	// magic numbers
	f.y -= CHUNKSIZE * 3.f;
	f.w += CHUNKSIZE;
	f.x -= CHUNKSIZE * 2.f;
	f.z += CHUNKSIZE;

	m_tileShader.setIntUniform(tileSheetHeightUniformLoc, m_tileSheetTexture.height);
	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f);

	int drawnChunkCount = 0;

	m_tileFBO.bind();
	m_tileFBO.setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	// prevent overdrawing
	static std::set<ChunkPos> seenPositions;
	ChunkPos pos;
	while (auto update = m_chunkUpdateObserver.observe()) {
		if (update.value().type == ChunkUpdateType::DONE_GENERATING || update.value().type == ChunkUpdateType::NEW_VBO_DATA) {
			pos = { update.value().x, update.value().y };
			seenPositions.insert(pos);
		};
	}
	for (ChunkPos pos : seenPositions) {
		drawnChunkCount += drawChunk(pos, p_windowWidth);
	}

	seenPositions.clear();

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

	m_tileShader.setBoolUniform(drawAsBordersUniformLoc, false);
	m_tileDrawStates.m_blendMode.disable();
	drawnChunkCount += redrawCameraView(chunkFrame);
	m_tileShader.setBoolUniform(drawAsBordersUniformLoc, true);
	m_tileDrawStates.m_blendMode.enable();
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
				if (!chunk.vboIsPushed) {
					chunk.pushVBO();
				}
				if (!chunk.tilesToSub.empty()) {
					chunk.subSingleTileVBOS();
				}

				m_tileDrawStates.attachShader(&m_tileShader);
				m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
				m_tileShader.use();

				m_tileFBO.bind();
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