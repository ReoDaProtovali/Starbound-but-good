#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "Framework/Window/GameWindow.hpp"
#include "GameConstants.hpp"

#include "Application.hpp"

#include "InitGL.h"

int main(int argc, char* argv[]) {
	LOG("Debug mode active!");

#ifdef SBBB_DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
#endif

	// must be init outside of application in order to get a valid gl context
	GameWindow gw = GameWindow("Barstound"); 
	initGL();

	Application sbbb = Application(gw);

	sbbb.run();
	sbbb.cleanUp();

	return 0;
}
