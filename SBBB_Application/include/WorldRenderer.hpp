#pragma once
#include "Chunk.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Camera.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "util/Messenger.hpp"

class WorldRenderer {

	FrameBuffer m_tileFBO;
	Sprite m_tileSprite;
	Shader m_tileShader{ ".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileGS.glsl", ".\\src\\Shaders\\TileFS.glsl" };

	std::shared_ptr<Camera> m_viewCam;
	Camera m_tileCam;
	glm::ivec4 m_chunkFramePrev;

	std::unordered_map<ChunkPos, WorldChunk*, ChunkPos> m_drawMap;
	Messenger<ChunkPos, WorldChunk*>& m_chunkMessenger = Messenger<ChunkPos, WorldChunk*>::Get();
	bool m_newInfoRecieved = false;

	void requestFrame();
	void checkForResponses();
	void tidy();

public:
	WorldRenderer();

	// If we want to draw the world, we kinda have to know where we are in it
	void setCamera(std::shared_ptr<Camera> p_cam);

	void draw(DrawSurface& p_surface);
};