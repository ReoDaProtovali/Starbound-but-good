#include "WorldRenderer.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
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

void WorldRenderer::setCamera(std::shared_ptr<Camera> p_cam)
{
	m_viewCam = p_cam;
}

int WorldRenderer::draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth)
{
	if (!m_viewCam) return 0;

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	p_surface.bind();
	m_tileSprite.attachTexture(m_tileFBO.getColorTex(0));
	m_tileSprite.draw(p_surface, p_states);

	auto f = m_viewCam->getFrame();
	f.y -= CHUNKSIZE * 4;
	f.w += CHUNKSIZE;
	f.z += CHUNKSIZE * 2;
	f.x -= CHUNKSIZE * 2;

	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f / 2.f) * 2;
	if (chunkFrame == m_chunkFramePrev) return 0;


	glm::ivec4 chunkFrameTiles = utils::frameToChunkCoords(f / 2.f) * CHUNKSIZE * 2;
	m_tileCam.setFrame(
		(float)chunkFrameTiles.x, (float)chunkFrameTiles.y, float(chunkFrameTiles.z - chunkFrameTiles.x), float(chunkFrameTiles.w - chunkFrameTiles.y)
	);

	m_tileSprite.setBounds(Rect(0, 0, m_tileCam.getFrameDimensions().x, m_tileCam.getFrameDimensions().y));
	m_tileSprite.setPosition(glm::vec3(m_tileCam.getFrame().x, m_tileCam.getFrame().w, 0));

	float pixelsPerTile = (float)p_windowWidth / (float)m_viewCam->tileScale;
	float pixelsPerTileMin = std::fminf(pixelsPerTile, 8.f);
	m_tileFBO.setDimensions(glm::vec2(pixelsPerTileMin * (chunkFrameTiles.z - chunkFrameTiles.x), pixelsPerTileMin * (chunkFrameTiles.w - chunkFrameTiles.y)));

	m_tileDrawStates.setTransform(m_tileCam.getTransform());
	m_tileFBO.clear();

	if (m_viewCam->tileScale > 500.f) {
		m_tileShader.setBoolUniform(4, false);
	}
	else {
		m_tileShader.setBoolUniform(4, true);
	}
	
	int drawnChunkCount = 0;
	for (int y = chunkFrame.y - 1; y <= chunkFrame.w + 1; y++) {
		for (int x = chunkFrame.x - 1; x <= chunkFrame.z + 1; x++) {
			if (s_chunkMap.contains(ChunkPos(x, y))) {
				WorldChunk& chunk = s_chunkMap[ChunkPos(x, y)];
				if (!chunk.drawable) continue;
				if (chunk.isEmpty) continue;
				if (!chunk.vboIsPushed) {
					chunk.pushVBO();
				}
				m_tileShader.setVec2Uniform(2, glm::vec2(chunk.worldPos.x, chunk.worldPos.y));
				chunk.draw(m_tileFBO, m_tileDrawStates);

				drawnChunkCount++;
			};
		}
	}


	m_chunkFramePrev = chunkFrame;
	return drawnChunkCount;
}
