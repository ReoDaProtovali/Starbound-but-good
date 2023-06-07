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

	Camera* m_viewCam = nullptr;
	Camera m_tileCam;
	glm::ivec4 m_chunkFramePrev;

    SharedMap<ChunkPos, WorldChunk, ChunkPos>& s_chunkMap = SharedMap<ChunkPos, WorldChunk, ChunkPos>::Get();

	void tidy();
	int redrawCameraView(const glm::vec4& chunkFrame);

public:
	WorldRenderer();
	Sprite m_tileSprite = Sprite(glm::vec3(-1, -1, 0), Rect(0, 0, 10, 10));
	float m_pixelsPerTile = 8.f;
	Observer<ChunkUpdate> m_chunkUpdateObserver;

	// If we want to draw the world, we kinda have to know where we are in it
	void setCamera(Camera* p_cam);
	bool drawChunkBorders = false;
	int draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth);
};