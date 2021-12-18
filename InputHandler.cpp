#include "InputHandler.hpp"
void InputHandler::processKeyDown(int p_sym) {
	if (!std::count(keys.begin(), keys.end(), p_sym)) {
		keys.push_back(p_sym);
	}
};
void InputHandler::processKeyUp(int p_sym) {
	for (int i = 0; i < keys.size(); i++) {
		if (keys[i] == p_sym) {
			keys.erase(keys.begin() + i);
			break;
		}
	}
};
bool InputHandler::testKey(int p_sym) {
	if (std::count(keys.begin(), keys.end(), p_sym)) {
		return true;
	}
	return false;
};