#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "Application.hpp"

int main(int argc, char* argv[]) {

#ifdef SBBB_DEBUG
	LOG("Debug mode active!");
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
#endif

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	Application sbbb = Application();

	sbbb.run();

	return 0;
}
