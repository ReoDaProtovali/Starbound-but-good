#pragma once
#include <SDL.h>
#include <vector>
#include <algorithm>
#include <iostream>

class InputHandler
{
public:
	InputHandler() {};
	void processKeyDown(int p_sym);
	void processKeyUp(int p_sym);
	bool testKey(int p_sym);
private:
	std::vector<int> keys;
};

