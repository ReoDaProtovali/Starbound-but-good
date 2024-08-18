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
public:

	SharedMap<ChunkPos, WorldChunk, ChunkPos>& s_chunkMap = SharedMap<ChunkPos, WorldChunk, ChunkPos>::Get();
	void tidy();
	int redrawTilesView(const glm::vec4& chunkFrame);
	int redrawBordersView(const glm::vec4& chunkFrame);

	WorldRenderer();
	int drawChunk(ChunkPos pos, uint32_t p_windowWidth);

	// If we want to draw the world, we kinda have to know where we are in it
	void setCamera(Camera* p_cam);

	Lighting lighting;
	glm::ivec4 currentTileFrame{0.f, 0.f, 1.f, 1.f};

	bool drawChunkBorders = false;
	int draw(DrawSurface& p_surface, DrawStates& p_states, uint32_t p_windowWidth);
	float pixelsPerTile = 8.f;

private:

	FrameBuffer m_tileFBO;
	FrameBuffer m_borderFBO;
	const char* varyingNames[3] = { "TexCoord", "zLevel", "pos" };
	Shader m_tileShader{ ".\\src\\Shaders\\TileVS.glsl", ".\\src\\Shaders\\TileFS.glsl" };
	GLuint tileSheetHeightUniformLoc = 0;
	GLuint worldPosUniformLoc = 0;
	GLuint tileSheetUniformLoc = 0;
	GLuint drawAsBordersUniformLoc = 0;
	Texture m_tileSheetTexture;

	//Shader m_tileFeedbackShader{ ".\\src\\Shaders\\TileFeedbackVS.glsl", ".\\src\\Shaders\\TileFS.glsl" };
	// different shaders need different uniforms
	GLuint feedback_tileSheetUniformLoc = 0;
	GLuint feedback_tileSheetHeightUniformLoc = 0;
	GLsync drawFence = 0;

	DrawStates m_tileDrawStates;
	Camera m_tileCam;
	Camera* m_viewCam = nullptr;
	glm::ivec4 m_chunkFramePrev;
	Sprite m_tileSprite = Sprite(glm::vec3(-1, -1, 0), Rect(0, 0, 10, 10));

	Observer<ChunkUpdate> m_chunkUpdateObserver{ globals.chunkUpdateSubject };
	GenericShaders& gs = GenericShaders::Get();

};