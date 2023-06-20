#include "Application.hpp"
#include <thread>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
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
				localServer.stateManager.close();
				client.stop();
				localServer.stop();
			}
			break;
		}
		if (client.clientStopping || localServer.serverStopping) break;

		if (!gameActive) break;
	}

	localServer.stateManager.close();
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
	// workaround
	static Messenger<SDL_Event, int>& SDLEventMessenger = Messenger<SDL_Event, int>::Get();
	while (SDL_PollEvent(&event)) {
		SDLEventMessenger.sendMessageFront((SDL_Event)event);

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
					(float)event.motion.x / client.gw.width,
					(float)event.motion.y / client.gw.height,
					(float)event.motion.x,
					(float)event.motion.y,
					false,
					false,
					true,
					0,
					event.motion.state
				});
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.window.windowID != client.getWindowID()) break;
			mouseMessenger.sendMessageFront(MouseEvent{
					(float)event.motion.x / client.gw.width,
					(float)event.motion.y / client.gw.height,
					(float)event.motion.x,
					(float)event.motion.y,
					true,
					false,
					false,
					event.button.button,
					0
				});
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.window.windowID != client.getWindowID()) break;
			mouseMessenger.sendMessageFront(MouseEvent{
					(float)event.motion.x / client.gw.width,
					(float)event.motion.y / client.gw.height,
					(float)event.motion.x,
					(float)event.motion.y,
					false,
					true,
					false,
					event.button.button,
					0
				});
			break;
		}
	}
}
