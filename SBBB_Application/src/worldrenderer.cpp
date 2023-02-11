#include "WorldRenderer.hpp"

void WorldRenderer::tidy()
{
}

WorldRenderer::WorldRenderer() 
{
	m_chunkFramePrev = glm::ivec4(0);
	m_tileCam.pos = glm::vec3(-1.f, -1.f, 32.0f);
	m_tileCam.tileScale = 40.0f;
	m_tileCam.disableAutoFrame();

	m_tileSprite.attachTexture(m_tileFBO.getColorTex(0));

}

void WorldRenderer::setCamera(std::shared_ptr<Camera> p_cam)
{
	m_viewCam = p_cam;
}

void WorldRenderer::requestFrame()
{
	auto f = m_viewCam->getFrame();
	f.y -= CHUNKSIZE * 4;
	f.w += CHUNKSIZE;
	f.z += CHUNKSIZE * 2;
	f.x -= CHUNKSIZE * 2;

	glm::ivec4 chunkFrame = utils::frameToChunkCoords(f / 2.f) * 2;
	if (chunkFrame == m_chunkFramePrev) return;

	for (int y = chunkFrame.y - 1; y <= chunkFrame.w + 1; y++) {
		for (int x = chunkFrame.x - 1; x <= chunkFrame.z + 1; x++) {
			LOG("Requested " << x << " " << y);
			m_chunkMessenger.sendMessageFront(ChunkPos(x, y));
		}
	}

	m_chunkFramePrev = chunkFrame;

}

void WorldRenderer::checkForResponses()
{
	while (auto opt = m_chunkMessenger.getMessageBack()) {
		WorldChunk* chunk = opt.value();
		// ignore chunks that the manager does not have
		if (!chunk) continue;
		m_drawMap.insert(std::make_pair(chunk->worldPos, chunk));
	}
}

void WorldRenderer::draw(DrawSurface& p_surface)
{
	if (!m_viewCam) return;
	requestFrame();
	checkForResponses();
}
