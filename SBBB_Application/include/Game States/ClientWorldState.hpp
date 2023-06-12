#pragma once
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "WorldRenderer.hpp"
#include "GameStates.hpp"
#include "GUI.hpp"
#include "GUITextField.hpp"
#include "GUIDragBar.hpp"

#define BOMB_COUNTER_MAX 50

class Lighting;
class ClientWorldState : public GameState {
public:
	ClientWorldState() = delete;
	ClientWorldState(GameWindow& p_window, GameRenderer& p_renderer) :
		renderer(p_renderer) {};

	// Temporary until a better texture loader is devised.
	void loadTextures();

	SharedList<EntityWrapper>& entities = SharedList<EntityWrapper>::Get();
	Subject<TileUpdateRequest>& m_tileRequester = Subject<TileUpdateRequest>::Get();

	void init() override;
	void update() override;
	void suspend() override;
	void resume() override;
	void close() override;

	int drawWorld();

	// ------------------------------------------------------------------
	Camera* playerCam = nullptr;

	GUI& gui = GUI::Get();
	Sprite testReoSprite = Sprite(glm::vec3(-16.0f, 109.0f, 2.0f), Rect(0.f, 0.f, 3.f, 3.f));
	Texture* testReoTexture = nullptr;

	Sprite bombSprite = Sprite(glm::vec3(9999.f, 9999.f, 2.f), Rect(0.f, 0.f, 1.f, 1.f));
	int bombCounter = BOMB_COUNTER_MAX;

	uint64_t testFrame = 0;
	void testDraw();
	GUIButton testButton{"testbutton", 0.4f, 0.1f, 0.3f, 0.1f};
	GUITextField testButtonText{ "buttontext", Rect(0.f, 0.f, 1.f, 1.f) };
	GUITextField testText{ "testtext", Rect(0.1f, 0.0f, 0.3f, 1.0f) };
	GUITextField testTextAbsolute{ "testtextabs", Rect(0.1f, 0.0f, 0.3f, 1.0f) };
	GUIDragBar testDragBar{ "testdragbar", Rect(0.f, 0.f, 1.f, 0.15f) };
	GUIDragBar testDragBar2{ "testdragbar2", Rect(0.f, 0.f, 1.f, 0.15f) };
	GUIDragBar testDragBar3{ "testdragbar2", Rect(0.f, 0.f, 1.f, 0.15f) };

	GUITextField fpsTextField{ "fpstext", Rect(0.05f, 0.05f, 0.2f, 0.1f) };

	// ----------------------------------------------------------------------

private:

	WorldRenderer worldRenderer;
	GameRenderer& renderer;
	Lighting m_lighting;

	Camera m_followerCam; // in world space

	Observer<MouseEvent> m_mouseObserver;
	GenericShaders& gs = GenericShaders::Get();
	ResourceManager& res = ResourceManager::Get();

};