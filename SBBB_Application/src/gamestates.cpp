#include "GameStates.hpp"

//void GameStateManager::clientUpdate()
//{
//	if (!m_activeState->ready() || !m_activeState->clientState->active) return;
//	m_activeState->clientState->update();
//
//}
//
//void GameStateManager::serverUpdate()
//{
//	if (!m_activeState->ready() || !m_activeState->serverState->active) return;
//	m_activeState->clientState->update();
//}
//
//// this stuff is a bit dumb
//void GameStateManager::bindClientState(GameStateEnum p_ID, GameState* p_clientState)
//{
//	if (stateExists(p_ID)) {
//		getState(p_ID)->clientState = p_clientState;
//	}
//	m_allStates.push_back(GameStatePair());
//	m_allStates[m_allStates.size() - 1].clientState = p_clientState;
//}
//
//void GameStateManager::bindServerState(GameStateEnum p_ID, GameState* p_serverState)
//{
//	if (stateExists(p_ID)) {
//		getState(p_ID)->serverState = p_serverState;
//	}
//	m_allStates.push_back(GameStatePair());
//	m_allStates[m_allStates.size() - 1].serverState = p_serverState;
//}
//
//void GameStateManager::swap(GameStateEnum newState)
//{
//	if (!m_activeState->ready()) throw std::exception("GameState has not been set up! Cannot swap existing state.");
//	m_activeState->clientState->close();
//	m_activeState->serverState->close();
//	m_activeState->clientState->active = false;
//	m_activeState->serverState->active = false;
//
//	m_activeState = getState(newState);
//
//	m_activeState->serverState->init();
//	m_activeState->clientState->init();
//	m_activeState->clientState->active = true;
//	m_activeState->serverState->active = true;
//
//}
//
//bool GameStateManager::stateExists(GameStateEnum p_ID)
//{
//	for (auto& s : m_allStates) {
//		if (s.ID == p_ID) return true;
//	}
//	return false;
//}
//
//GameState::GameState()
//{
//
//}
