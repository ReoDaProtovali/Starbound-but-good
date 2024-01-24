#include "Game States/MenuState.hpp"

void MenuState::init()
{

}

void MenuState::update()
{
	ImGui::Begin("Menu.");
	if (ImGui::Button("Play.", ImVec2(100, 40))) {
		GameStateManager& gs = GameStateManager::Get();
		gs.swap(GameStateEnum::IN_WORLD);
	}
	ImGui::End();
}

void MenuState::close()
{
}
