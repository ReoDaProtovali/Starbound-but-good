#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"

GameClient::GameClient() {
	stateManager.bindClientState(GameStateEnum::NO_STATE, (GameState*)&State_None);
	stateManager.bindClientState(GameStateEnum::IN_WORLD, (GameState*)&State_ClientWorld);
	stateManager.setState(GameStateEnum::IN_WORLD);
	gw.setVSync(true);
	imctx = ImGui::CreateContext();
}
GameClient::~GameClient() {
	gw.cleanUp();
}

void GameClient::start(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	// important
	gw.unbindFromThisThread();
	clientThread = std::thread(&GameClient::run, this, std::ref(p_exceptionQueue));
}

void GameClient::stop() {
	stopping = true;
	clientThread.join();
}
void GameClient::run(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	Messenger<MouseEvent, int>& mouseMessenger = Messenger<MouseEvent, int>::Get(); // one-way messenger for capturing mouse events
	Subject<MouseEvent>& mouseSubject = globals.mouseSubject; // for re-transmitting mouse events to client

	Observer<KeyEvent> keyObserver{ globals.keySubject };
	gw.bindToThisThread();

	ImGui::SetCurrentContext(imctx);
	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(gw.m_window, gw.m_glContext);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);

	try {

		auto& res = ResourceManager::Get();
		auto& gs = GenericShaders::Get();

		while (true) {

			if (gw.hasChangedFullscreenState()) {
				resizeWindow(gw.width, gw.height);
			}
			if (flagResize) {
				flagResize = false;
				resizeWindow(newWidth, newHeight);
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();


			static bool open = true;
			ImGui::ShowDemoWindow(&open);

			static Messenger<SDL_Event, int>& SDLEventMessenger = Messenger<SDL_Event, int>::Get();
			while (auto e = SDLEventMessenger.getMessageFront()) {
				ImGui_ImplSDL2_ProcessEvent(&e.value());
			}

			renderFPSGauge.update(0.5f);
			testInput();
			processDebugStats();

			static GUIEvent e; // static so it keeps mouse pos between updates
			bool needsSent = true;

			// reset mouse state if we have a new update, otherwise hold current state.
			//if (mouseMessenger.incomingFront()) {
			//	e.mouse = MouseEvent();
			//}
			// compress mouse events, lots of redundancy

			while (auto opt = keyObserver.observe()) {
				if (opt.value().wasDown) {
					e.key = opt.value();
					e.key.valid = true;
					gui.update(e);
					needsSent = false;
				}
			}
			while (auto opt = mouseMessenger.getMessageFront()) {
				e.mouse = opt.value();
				if (!gui.update(e))
					mouseSubject.notifyAll(e.mouse);
				needsSent = false;
			}
			if (needsSent) {
				if (!gui.update(e)) { 
					mouseSubject.notifyAll(e.mouse);
				};
			}
			gw.clear();
			stateManager.clientUpdate();
			gui.draw(gw);
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			ImGui::Render();

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
				SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
			}

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			gw.displayNewFrame();

			if (stateManager.maybeStopClient()) stopping = true;
			if (stopping) break;
		}

	} catch (std::exception& ex) {
		ERROR_LOG("Exception in " << __FILE__ << " at " << __LINE__ << ": " << ex.what());
		p_exceptionQueue.push(std::current_exception());
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext(imctx);
		gw.cleanUp();
		return;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext(imctx);
	gw.cleanUp();
}


void GameClient::resizeWindow(uint32_t p_w, uint32_t p_h)
{
	gw.width = p_w;
	gw.height = p_h;
	gw.setViewport(0, 0, p_w, p_h);
	renderer.setViewport(p_w, p_h);
}

void GameClient::testInput()
{
	static Observer<MouseEvent> mouseObserver{ globals.mouseSubject };
	Camera& cam = *renderer.cam;

	float camSpeed = 0.02f;
	if (inp.testKey(SDLK_w)) {
		camVelocity.y += camSpeed;
	}
	if (inp.testKey(SDLK_a)) {
		camVelocity.x -= camSpeed;
	}
	if (inp.testKey(SDLK_s)) {
		camVelocity.y -= camSpeed;
	}
	if (inp.testKey(SDLK_d)) {
		camVelocity.x += camSpeed;
	}

	if (inp.testKey(SDLK_q)) {
		cam.tileScale *= 0.992f;
		cam.updateFrame();
	}
	if (inp.testKey(SDLK_e)) {
		cam.tileScale *= 1.01f;
		cam.updateFrame();
	}

	//if (inp.testKeyDown(SDLK_b)) {
	//	stateManager.swap(GameStateEnum::NO_STATE);
	//}

	//if (inp.testKeyDown(SDLK_3)) {
	//	renderer.swapCameras();
	//}

	if (inp.testKeyDown(SDLK_5)) {
		gw.toggleFullscreen();
	}
	camVelocity *= 0.95;
	cam.pos += glm::vec3(camVelocity.x, camVelocity.y, 0.f);

	//while (auto obs = mouseObserver.observe()) {
	//	MouseEvent m = obs.value();
	//	m.x = m.x / (float)gw.width;
	//	m.y = m.y / (float)gw.height;
	//	renderer.testButton.onUpdate(GUIEvent{ m, KeyEvent() });
	//}
}

void GameClient::processDebugStats()
{
	globals.drawFPS = (float)(1.0 / renderFPSGauge.getFrametimeAverage());
#ifndef DISABLE_DEBUG_STATS
	static uint32_t debugUpdateCounter = 0;
	debugUpdateCounter++;
	if (debugUpdateCounter > FRAMES_BETWEEN_STAT_UPDATES) { 
		debugUpdateCounter = 0;
		DebugStats& db = globals.debug;
		//db.updateFPS = 1.0f / utils::averageVector(updateFPSGauge.frametimeBuffer);
		db.camX = renderer.cam->pos.x;
		db.camY = renderer.cam->pos.y;
		auto f = renderer.cam->getFrame();
		db.camFX1 = f.x;
		db.camFY1 = f.y;
		db.camFX2 = f.z;
		db.camFY2 = f.w;
		db.screenW = renderer.window.screenWidth;
		db.screenH = renderer.window.screenHeight;
		db.windowW = renderer.window.width;
		db.windowH = renderer.window.height;
		db.statUpdate = true;
	}
#endif
}
