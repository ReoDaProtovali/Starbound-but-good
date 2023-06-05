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
	m_tileFBO.getColorTex(0)->setFiltering(GL_NEAREST, GL_NEAREST);
	m_tileSprite.attachTexture(m_tileFBO.getColorTex(0));

	auto& res = ResourceManager::Get();
	res.loadAllTileSets();
	Texture* tilesheet = res.getTileSheetTexture();

	m_tileShader.setTexUniform("tileSheet", 0);
	m_tileShader.setIntUniform(1, tilesheet->height);

	m_tileDrawStates.attachTexture(res.getTileSheetTexture());

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
		m_tileShader.setBoolUniform(4, false);
	}
	else {
		m_tileShader.setBoolUniform(4, true);
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
	static std::set<ChunkPos> seenPositions;

	while (auto update = m_chunkUpdateObserver.observe()) {
		if (update.value().type == ChunkUpdateType::DONE_GENERATING || update.value().type == ChunkUpdateType::NEW_VBO_DATA) {
			ChunkPos pos{ update.value().x, update.value().y };
			seenPositions.insert(pos);
		};
	}

	for (ChunkPos pos : seenPositions) {
		float pixelsPerTile = (float)p_windowWidth / (float)m_viewCam->tileScale;

		// has to be shifted one chunk down on the y for reasons
		auto pixelCoords = m_tileCam.tileToPixelCoordinates(pos.x * (float)CHUNKSIZE, pos.y * (float)CHUNKSIZE - (float)CHUNKSIZE);
		m_tileFBO.clearRegion((int)roundf(pixelCoords.x), (int)roundf(pixelCoords.y), (int)roundf(CHUNKSIZE * m_pixelsPerTile), (int)roundf(CHUNKSIZE * m_pixelsPerTile));
		m_tileFBO.clearDepthRegion((int)roundf(pixelCoords.x), (int)roundf(pixelCoords.y), (int)roundf(CHUNKSIZE * m_pixelsPerTile), (int)roundf(CHUNKSIZE * m_pixelsPerTile));
		if (s_chunkMap.contains(pos)) {

			WorldChunk& chunk = s_chunkMap[pos];
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
			m_tileShader.setVec2Uniform(2, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
			chunk.draw(m_tileFBO, m_tileDrawStates);
			if (drawChunkBorders) {
				SBBBDebugDraw::drawBoxImmediate(chunk.getPosition().x, chunk.getPosition().y, CHUNKSIZE, CHUNKSIZE, glm::vec3(0.f, 0.f, 1.f), m_tileFBO, m_tileCam);
			}
			drawnChunkCount++;
		}
	}

	seenPositions.clear();
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
	int drawnChunkCount = 0;
	for (int y = (int)chunkFrame.y - 1; y <= (int)chunkFrame.w + 1; y++) {
		for (int x = (int)chunkFrame.x - 1; x <= (int)chunkFrame.z + 1; x++) {
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
				auto pixelCoords = m_tileCam.tileToPixelCoordinates(x * (float)CHUNKSIZE, y * (float)CHUNKSIZE - (float)CHUNKSIZE);

				m_tileFBO.clearDepthRegion((GLint)roundf(pixelCoords.x), (GLint)roundf(pixelCoords.y), (GLsizei)roundf(CHUNKSIZE * m_pixelsPerTile), (GLsizei)roundf(CHUNKSIZE * m_pixelsPerTile));

				m_tileShader.setVec2Uniform(2, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
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