#pragma once
#include "Chunk.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Camera.hpp"
#include "Framework/Graphics/FrameBuffer.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Lighting.hpp"
#include "util/Messenger.hpp"
#include "util/DebugDraw.hpp"
#include "util/SharedQueue.hpp"
#include "util/SubjectObserver.hpp"
class WorldRenderer {

	FrameBuffer m_tileFBO;
	const char* varyingNames[3] = { "TexCoord", "zLevel", "pos" };
	Shader m_tileShader{ ".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileGS.glsl", ".\\src\\Shaders\\TileFS.glsl", varyingNames, 3 };
	GLuint tileSheetHeightUniformLoc = 0;
	GLuint worldPosUniformLoc = 0;
	GLuint tileSheetUniformLoc = 0;
	GLuint drawAsBordersUniformLoc = 0;

	Shader m_tileFeedbackShader{ ".\\src\\Shaders\\TileFeedbackVS.glsl", ".\\src\\Shaders\\TileFS.glsl" };
	// different shaders need different uniforms
	GLuint feedback_tileSheetUniformLoc = 0;
	GLuint feedback_tileSheetHeightUniformLoc = 0;

	Texture tilesheet;
	DrawStates m_tileDrawStates;
	DrawStates m_firstPassDrawStates;
	Camera* m_viewCam = nullptr;
	glm::ivec4 m_chunkFramePrev;

	Sprite m_tileSprite = Sprite(glm::vec3(-1, -1, 0), Rect(0, 0, 10, 10));
	float m_pixelsPerTile = 8.f;
    SharedMap<ChunkPos, WorldChunk, ChunkPos>& s_chunkMap = SharedMap<ChunkPos, WorldChunk, ChunkPos>::Get();
	GenericShaders& gs = GenericShaders::Get();
	void tidy();
	int redrawCameraView(const glm::vec4& chunkFrame);

public:
	Camera m_tileCam;
	WorldRenderer();
	int redrawCameraView();
	Lighting lighting;

	Observer<ChunkUpdate> m_chunkUpdateObserver{ globals.chunkUpdateSubject };

	// If we want to draw the world, we kinda have to know where we are in it
	void setCamera(Camera* p_cam);
	bool drawChunkBorders = false;
	int draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth);
};