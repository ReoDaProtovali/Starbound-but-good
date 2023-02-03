#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "Application.hpp"
#include "GameClient.hpp"

int main(int argc, char* argv[]) {
	// LOG("Debug mode active!");

#ifdef SBBB_DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // memory leak detection
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
#endif


	//GameClient client;

	//client.start();

	//std::this_thread::sleep_for(std::chrono::seconds(10));

	//client.stop();

	//return 0;

	Application sbbb = Application();

	sbbb.run();
	sbbb.cleanUp();

	return 0;
}
