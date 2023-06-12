#include "Application.hpp"
#include <thread>

Application::Application()
{
}


void Application::run()
{
	startClient();

	startServer();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_RATE_MS));
		pollEvents();

		// handle thread exceptions
		if (auto exp = m_exceptionQueue.tryPop()) {
			try {
				std::rethrow_exception(exp.value());
			}
			catch (std::exception& e) {
				ERROR_LOG("Main thread caught exception " << e.what());
				client.stop();
				localServer.stop();
			}
			break;
		}

		if (!gameActive) break;
	}

    client.stop();

	localServer.stop();
}

void Application::startClient()
{
	client.start(m_exceptionQueue);
}

void Application::startServer()
{
	localServer.start(m_exceptionQueue);
}

void Application::pollEvents()
{
	static SDL_Event event;
	//static Subject<MouseEvent>& mouseSubject = Subject<MouseEvent>::Get();
	static Messenger<MouseEvent, int>& mouseMessenger = Messenger<MouseEvent, int>::Get();

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_WINDOWEVENT:
			if (!(event.window.event == SDL_WINDOWEVENT_RESIZED)) break;
			printf("Window %u resized to %dx%d\n", event.window.windowID, event.window.data1, event.window.data2);
			if (event.window.windowID != client.getWindowID()) break;
			client.newWidth = event.window.data1;
			client.newHeight = event.window.data2;
			client.flagResize = true;
			break;
		case SDL_QUIT:
			gameActive = false;
			break;
		case SDL_KEYDOWN:
			client.inp.processKeyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			client.inp.processKeyUp(event.key.keysym.sym);
			if (event.key.keysym.sym == SDLK_ESCAPE) gameActive = false;
			break;
		case SDL_MOUSEMOTION:
			if (event.window.windowID != client.getWindowID()) break;
			mouseMessenger.sendMessageFront(MouseEvent{
					(float)event.motion.x,
					(float)event.motion.y,
					(float)event.motion.x,
					(float)event.motion.y,
					false,
					event.motion.state
				});
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.window.windowID != client.getWindowID()) break;
			mouseMessenger.sendMessageFront(MouseEvent{
					(float)event.motion.x,
					(float)event.motion.y,
					(float)event.motion.x,
					(float)event.motion.y,
					true,
					event.motion.state
				});
		}
	}
}
