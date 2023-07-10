#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
// BEGIN EPIC SPAGHETTI CODE
#include "Application.hpp" 

int main(int argc, char* argv[]) {

#ifdef SBBB_DEBUG
	LOG("Debug mode active!");
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
#endif

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	Application sbbb = Application();

	sbbb.run();

	return 0;
}
// END EPIC SPAGHETTI CODE