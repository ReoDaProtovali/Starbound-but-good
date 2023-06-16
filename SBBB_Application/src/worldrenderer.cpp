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

	m_tileFBO.setDimensions(glm::vec2(10.f, 10.f));
	m_tileFBO.getColorTex(0).setFiltering(GL_NEAREST, GL_NEAREST);
	m_tileSprite.attachTexture(m_tileFBO.getColorTex(0));

	auto& res = ResourceManager::Get();
	res.loadAllTileSets();
	tilesheet = res.getTileSheetTexture();

	glm::mat4 tmp = glm::mat4(1.f);
	m_tileShader.addMat4Uniform("transform", tmp);
	tileSheetHeightUniformLoc = m_tileShader.addIntUniform("tileSheetHeight", tilesheet.height);
	worldPosUniformLoc = m_tileShader.addVec2Uniform("worldPos", glm::vec2(0.f));
	tileSheetUniformLoc = m_tileShader.addTexUniform("tileSheet", 0);
	generateConnectionsUniformLoc = m_tileShader.addBoolUniform("generateConnections", 0);

	m_tileFeedbackShader.addMat4Uniform("transform", tmp);
	feedback_tileSheetUniformLoc = m_tileFeedbackShader.addTexUniform("tileSheet", 0);
	feedback_tileSheetHeightUniformLoc = m_tileFeedbackShader.addIntUniform("tileSheetHeight", tilesheet.height);

	Texture tileSheetTexture = res.getTileSheetTexture();
	m_tileDrawStates.attachTexture(tileSheetTexture);

	m_tileDrawStates.attachShader(&m_tileShader);
}

void WorldRenderer::setCamera(Camera* p_cam)
{
	m_viewCam = p_cam;
}

int WorldRenderer::draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth)
{
	if (!m_viewCam) return 0;
	glEnable(GL_DEPTH_TEST);
	p_surface.bind();
	m_tileSprite.attachTexture(m_tileFBO.getColorTex(0));
	m_tileSprite.setOrigin(glm::vec2(0.f));
	m_tileSprite.draw(p_surface, p_states);

	if (m_viewCam->tileScale > 500.f) {
		m_tileShader.setBoolUniform(generateConnectionsUniformLoc, false);
	}
	else {
		m_tileShader.setBoolUniform(generateConnectionsUniformLoc, true);
	}

	auto f = m_viewCam->getFrame();
	// magic numbers
	f.y -= CHUNKSIZE * 4;
	f.w += CHUNKSIZE;
	f.z += CHUNKSIZE * 2;
	f.x -= CHUNKSIZE * 2;

	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f / 2.f) * 2;

	int drawnChunkCount = 0;

	// prevent overdrawing
	//static std::set<ChunkPos> seenPositions;

	while (auto update = m_chunkUpdateObserver.observe()) {
		if (update.value().type == ChunkUpdateType::DONE_GENERATING || update.value().type == ChunkUpdateType::NEW_VBO_DATA) {
			ChunkPos pos{ update.value().x, update.value().y };
			// workaround to fix chunk border bug
			//for (int i = -1; i <= 1; i++) {
			//	for (int j = -1; j <= 1; j++) {
			//		if (i == 0 && j == 0) continue;
			//		auto neighborPos = ChunkPos(pos.x + i, pos.y + j);
			//		if (!s_chunkMap.contains(neighborPos)) continue;
			//		WorldChunk& c = s_chunkMap[neighborPos];
			//		if (c.invalid) continue;
			//		c.feedbackMeshReady = false;
			//	}
			//}
			drawnChunkCount += redrawCameraView(chunkFrame);
			m_chunkUpdateObserver.clear();
			break;
			//seenPositions.insert(pos);
		};
	}

	if (chunkFrame == m_chunkFramePrev) return 0;

	m_pixelsPerTile = std::fminf((float)p_windowWidth / (float)m_viewCam->tileScale, 8.f);

	glm::ivec4 chunkFrameTiles = utils::frameToChunkCoords(f / 2.f) * CHUNKSIZE * 2;
	// breaks the frame, must be set manually
	m_tileCam.setDimensions(int(m_pixelsPerTile * float(chunkFrameTiles.z - chunkFrameTiles.x)), int(m_pixelsPerTile * float(chunkFrameTiles.w - chunkFrameTiles.y)));
	m_tileCam.setFrame(
		(float)chunkFrameTiles.x, (float)chunkFrameTiles.y, float(chunkFrameTiles.z - chunkFrameTiles.x), float(chunkFrameTiles.w - chunkFrameTiles.y)
	);

	m_tileSprite.setBounds(Rect(0, 0, m_tileCam.getFrameDimensions().x, m_tileCam.getFrameDimensions().y));
	m_tileSprite.setPosition(glm::vec3(m_tileCam.getFrame().x, m_tileCam.getFrame().w, 0));


	m_tileFBO.setDimensions(glm::vec2(m_pixelsPerTile * (chunkFrameTiles.z - chunkFrameTiles.x), m_pixelsPerTile * (chunkFrameTiles.w - chunkFrameTiles.y)));
	m_tileDrawStates.setTransform(m_tileCam.getTransform());
	m_tileFBO.clear();


	drawnChunkCount += redrawCameraView(chunkFrame);
	//m_tileFBO.clearRegion(m_tileCam.tileToPixelCoordinates(0.f, -CHUNKSIZE).x, m_tileCam.tileToPixelCoordinates(0.f, -CHUNKSIZE).y, pixelsPerTileMin * CHUNKSIZE, pixelsPerTileMin * CHUNKSIZE);
	//auto test = m_tileCam.tileToPixelCoordinates(0.f, 30.f);

	m_chunkFramePrev = chunkFrame;
	return drawnChunkCount;
}

int WorldRenderer::redrawCameraView(const glm::vec4& chunkFrame)
{
	m_tileFBO.clear();
	int drawnChunkCount = 0;
	for (int y = (int)chunkFrame.y; y <= (int)chunkFrame.w; y++) {
		for (int x = (int)chunkFrame.x; x <= (int)chunkFrame.z; x++) {
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

				m_tileFBO.clearDepthRegion((GLint)roundf(pixelCoords.x), (GLint)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * m_pixelsPerTile), (GLsizei)roundf(CHUNKSIZE * m_pixelsPerTile));


				if (chunk.feedbackMeshReady) {
					m_tileDrawStates.attachShader(&m_tileFeedbackShader);
					m_tileFeedbackShader.setIntUniform(feedback_tileSheetHeightUniformLoc, tilesheet.height);
					m_tileFeedbackShader.setTexUniform(feedback_tileSheetUniformLoc, 0);
					m_tileFeedbackShader.use();
				}
				else {
					m_tileDrawStates.attachShader(&m_tileShader);
					m_tileShader.setVec2Uniform(worldPosUniformLoc, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
					m_tileShader.use();
				}
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