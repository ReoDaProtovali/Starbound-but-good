#pragma once
#include <SDL.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <mutex>
#include "util/SubjectObserver.hpp"
#include "GameConstants.hpp"
class InputHandler
{
public:
	InputHandler() {
	};
	void processKeyDown(int p_sym);
	void processKeyUp(int p_sym);
	bool testKey(int p_sym);
	bool testKeyDown(int p_sym);
private:
	std::vector<int> m_keys;
	std::vector<int> m_keyDowns;

	std::mutex m_inputMutex;
	Subject<KeyEvent>& m_keySubject = Subject<KeyEvent>::Get();
};

