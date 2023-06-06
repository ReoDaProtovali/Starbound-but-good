#pragma once
#include <vector>
#include <unordered_map>

enum class GameStateEnum {
	NO_STATE,
	MENU,
	IN_WORLD
};

class GameState {
public:
	GameState() {};
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void suspend() = 0;
	virtual void close() = 0;

	bool initialized = false;
};
struct GameStatePair {
	GameState* clientState = nullptr;
	GameState* serverState = nullptr;
	bool ready() { return clientState && serverState; };
	GameStateEnum ID = GameStateEnum::NO_STATE;
};

class GameStateManager { // todo: un-gunch it by putting stuff in the cpp file
public:
	GameStateManager() {};

	void clientUpdate() {
		if (!m_activeState || !m_activeState->ready()) return;
		if (!m_activeState->clientState->initialized) {
			if (m_activeState->serverState->initialized) {
				m_activeState->clientState->init();
				m_activeState->clientState->initialized = true;
			}
			return;
		};
		m_activeState->clientState->update();
	};
	void serverUpdate() {
		if (!m_activeState || !m_activeState->ready()) return;
		if (!m_activeState->serverState->initialized) { // scuffed, but client won't start unless server
			m_activeState->serverState->init();
			m_activeState->serverState->initialized = true;
		}
		m_activeState->serverState->update();
	};

	void bindClientState(GameStateEnum p_ID, GameState* p_clientState) {
		if (stateExists(p_ID)) {
			getState(p_ID)->clientState = p_clientState;
		}
		m_allStates.push_back(GameStatePair());
		m_allStates[m_allStates.size() - 1].clientState = p_clientState;
		m_allStates[m_allStates.size() - 1].ID = p_ID;
	};
	void bindServerState(GameStateEnum p_ID, GameState* p_serverState) {
		if (stateExists(p_ID)) {
			getState(p_ID)->serverState = p_serverState;
		}
		m_allStates.push_back(GameStatePair());
		m_allStates[m_allStates.size() - 1].serverState = p_serverState;
		m_allStates[m_allStates.size() - 1].ID = p_ID;
	};

	void setState(GameStateEnum p_ID) { // only to be used for setting the very first state, really. It's a forceful set.
		if (stateExists(p_ID)) {
			m_activeState = getState(p_ID);
		}
	}
	void swap(GameStateEnum newState) {
		if (!m_activeState->ready()) throw std::exception("GameState has not been set up! Cannot swap existing state.");
		m_activeState->clientState->close();
		m_activeState->serverState->close();
		m_activeState->clientState->initialized = false;
		m_activeState->serverState->initialized = false;

		m_activeState = getState(newState);

		m_activeState->serverState->init();
		m_activeState->clientState->init();
	};
	void close() {
		if (!m_activeState) return;
		if (m_activeState->clientState)
			m_activeState->clientState->close();
		if (m_activeState->serverState)
			m_activeState->serverState->close();
	}
	static GameStateManager& Get() {
		static GameStateManager instance;
		return instance;
	}

	GameStatePair* getState(GameStateEnum p_ID) {
		for (auto& s : m_allStates) {
			if (s.ID == p_ID) return &s;
		}
		throw std::exception("State not found.");
		return nullptr;
	};

	bool stateExists(GameStateEnum p_ID) {
		for (auto& s : m_allStates) {
			if (s.ID == p_ID) return true;
		}
		return false;
	};
private:
	std::vector<GameStatePair> m_allStates;
	GameStatePair* m_activeState = nullptr;
};