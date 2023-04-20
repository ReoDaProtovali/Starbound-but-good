#pragma once
#include "Chunk.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Camera.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "util/Messenger.hpp"
#include "util/DebugDraw.hpp"
#include "util/SharedQueue.hpp"
#include "util/SubjectObserver.hpp"
class WorldRenderer {

	FrameBuffer m_tileFBO;
	Shader m_tileShader{ ".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileGS.glsl", ".\\src\\Shaders\\TileFS.glsl" };
	DrawStates m_tileDrawStates;

	std::shared_ptr<Camera> m_viewCam;
	Camera m_tileCam;
	glm::ivec4 m_chunkFramePrev;

    SharedMap<ChunkPos, WorldChunk, ChunkPos>& s_chunkMap = SharedMap<ChunkPos, WorldChunk, ChunkPos>::Get();

	void tidy();

public:
	WorldRenderer();
	Sprite m_tileSprite = Sprite(glm::vec3(-1, -1, 5), Rect(0, 0, 10, 10));

	Observer<ChunkUpdate> m_chunkUpdateObserver;

	// If we want to draw the world, we kinda have to know where we are in it
	void setCamera(std::shared_ptr<Camera> p_cam);
	bool drawChunkBorders = false;
	int draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth);
};