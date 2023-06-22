#pragma once
#include "Framework/Window/GameWindow.hpp"
#include "GameRenderer.hpp"
#include "WorldRenderer.hpp"
#include "GameStates.hpp"
#include "GUI.hpp"
#include "GUITextField.hpp"
#include "GUIDragBar.hpp"
#include "GUIContainer.hpp"
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
	Subject<TileUpdateRequest>& m_tileRequester = globals.tileUpdateRequestSubject;

	void init() override;
	void update() override;
	void suspend() override;
	void resume() override;
	void close() override;

	int drawWorld();

	// ------------------------------------------------------------------
	Camera* playerCam = nullptr;

	GUI& gui = GUI::Get();

	Sprite bombSprite = Sprite(glm::vec3(9999.f, 9999.f, 2.f), Rect(0.f, 0.f, 1.f, 1.f));
	int bombCounter = BOMB_COUNTER_MAX;

	glm::vec2 mousePos;
	DrawStates state;
	DrawStates worldDrawnState;
	uint64_t testFrame = 0;
	void testDraw();

	GUITextField fpsTextField{ "fpstext" };
	GUIDragBar fpsDragBar{ "fpsdragbar" };

	GUITextField debugTextField{ "debugtext" };
	GUIDragBar debugDragBar{ "debugdragbar" };

	GUIContainer win95Box{ "win95box" };
	GUITextField win95BoxText{ "win95Boxtext" };
	GUIContainer navBarImage{ "imagetest" };
	GUIContainer funnyButtonContainer { "funnybuttonc" };
	GUIButton funnyButton{ "funnybutton", 0.f, 0.f, 1.f, 1.f };
	GUIDragBar navDragBar{ "navdragbar" };

	GUIContainer tileSheetContainer{ "tsc" };
	// ----------------------------------------------------------------------

private:

	WorldRenderer worldRenderer;
	GameRenderer& renderer;

	Camera m_worldCam; // in world space

	Observer<MouseEvent> m_mouseObserver{ globals.mouseSubject };
	GenericShaders& gs = GenericShaders::Get();
	ResourceManager& res = ResourceManager::Get();

};