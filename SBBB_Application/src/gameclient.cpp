#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"
#include "Framework/Audio/wav.hpp"
#include "Framework/Audio/AudioImmediate.hpp"


GameClient::GameClient() {
	stateManager.bindClientState(GameStateEnum::NO_STATE, (GameState*)&State_None);
	stateManager.bindClientState(GameStateEnum::IN_WORLD, (GameState*)&State_ClientWorld);
	stateManager.bindClientState(GameStateEnum::MENU, (GameState*)&State_Menu);
	stateManager.setStateByForce(GameStateEnum::MENU);
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
	clientStopping = true;
	clientThread.join();
}

void audioInit();

void GameClient::run(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	SharedQueue<MouseEvent>& s_mouseQueue = SharedQueue<MouseEvent>::Get(); // one-way messenger for capturing mouse events
	SharedQueue<KeyEvent>& s_keyQueue = SharedQueue<KeyEvent>::Get(); // one-way messenger for capturing mouse events
	SharedQueue<SDL_Event>& s_SDLEventMessenger = SharedQueue<SDL_Event>::Get();

	Subject<MouseEvent>& mouseSubject = globals.mouseSubject; // for re-transmitting mouse events to client
	Subject<KeyEvent>& keySubject = globals.keySubject; // for re-transmitting key events to client

	gw.bindToThisThread();

	ImGui::SetCurrentContext(imctx);
	IMGUI_CHECKVERSION();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//dio.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

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

	audioInit();

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

			const char* testAudio[] = {
				"./res/HEY.wav",
				"./res/flup.wav",
				"./res/plong.wav",
				"./res/section.wav"
			};
			static size_t audio_device_dropdown_index = 0;
			static size_t test_audio_index = 0;
			const char* combo_preview_value = globals.audioDeviceNames[audio_device_dropdown_index].c_str();
			const char* test_audio_sample = testAudio[test_audio_index];
			bool device_changed = false;
			ImGui::Begin("Audio");


			if (ImGui::BeginCombo("Sample file", test_audio_sample)) {
				for (size_t i = 0; i < sizeof(testAudio) / sizeof(const char*); ++i) {
					const bool is_selected = test_audio_index == i;
					if (ImGui::Selectable(testAudio[i], &is_selected)) {
						test_audio_index = i;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Play test audio")) {
				play_wav_immediate(test_audio_sample, 1.f, 1.f, false, globals.alctx);
			}
			if (ImGui::BeginCombo("Audio Device", combo_preview_value)) {
				for (size_t i = 0; i < globals.audioDeviceNames.size(); ++i) {
					const bool is_selected = audio_device_dropdown_index == i;
					if (ImGui::Selectable(globals.audioDeviceNames[i].c_str(), &is_selected)) {
						device_changed = true;
						audio_device_dropdown_index = i;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::End();

			if (device_changed) {
				alcMakeContextCurrent(NULL);
				alcDestroyContext(globals.alctx);
				globals.alctx = alcCreateContext(alcOpenDevice(globals.audioDeviceNames[audio_device_dropdown_index].c_str()), nullptr);

				if (!alcMakeContextCurrent(globals.alctx)) {
					std::cerr << "Failed to set active context.\n";
				}
			}
			//static bool open = true;
			//ImGui::ShowDemoWindow(&open);

			while (auto e = s_SDLEventMessenger.tryPop()) {
				ImGui_ImplSDL2_ProcessEvent(&e.value());
			}

			renderFPSGauge.update(0.99f);
			testInput();
			processDebugStats();

			static GUIEvent e; // static so it keeps mouse pos between updates

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			e.key.valid = false;
			while (auto opt = s_keyQueue.tryPop()) {
				if (io.WantCaptureKeyboard) {
					break;
				}
				e.key.valid = true;
				keySubject.notifyAll(opt.value());
				if (opt.value().wasDown) {
					inp.processKeyDown(opt.value().keyCode);
				}
				else {
					inp.processKeyUp(opt.value().keyCode);
				}
				e.key = opt.value();
				gui.update(e);
			}

			while (auto opt = s_mouseQueue.tryPop()) {
				e.mouse = opt.value();
				if (!io.WantCaptureMouse && !gui.update(e)) {
					mouseSubject.notifyAll(e.mouse);
				};
			}


			gw.clear();
			stateManager.clientUpdate();
			gui.draw(gw);
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

			if (stateManager.maybeStopClient()) clientStopping = true;
			if (clientStopping) break;
		}

	}
	catch (std::exception& ex) {
		ERROR_LOG("Exception in " << __FILE__ << " at " << __LINE__ << ": " << ex.what());
		p_exceptionQueue.push(std::current_exception());
		cleanUp();
		return;
	}
	cleanUp();
}

void GameClient::cleanUp()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(globals.alctx);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext(imctx);
}

void GameClient::resizeWindow(uint32_t p_w, uint32_t p_h)
{
	gw.width = p_w;
	gw.height = p_h;
	gw.setViewport(0, 0, p_w, p_h);
	renderer.setViewport(p_w, p_h);
}

void audioInit() {

	const ALchar* allDevice_ptr = nullptr;
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE) { // able to get every device via openAL
		// openAL context owns this string so we don't need to free
		allDevice_ptr = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
		if (*allDevice_ptr != '\0') {
			globals.audioDeviceNames.push_back(reinterpret_cast<const char*>(allDevice_ptr));
		}
		// world's worst failsafe but at least it stops a runoff
		constexpr uint16_t MAX_DEVICE_CHARLEN = 1024;
		for (uint16_t itr = 0; !(allDevice_ptr[itr] == '\0' && allDevice_ptr[itr + 1] == '\0') && itr < MAX_DEVICE_CHARLEN; ++itr) {
			if (allDevice_ptr[itr] == '\0') {
				// copy from cstr to vector (maybe unsafe)
				globals.audioDeviceNames.push_back(reinterpret_cast<const char*>(allDevice_ptr + itr + 1));
			}
		}
	}

	const ALchar* defaultDevice_str = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	ALCdevice* defaultDevice = alcOpenDevice(defaultDevice_str);
	std::cout << "Got \"" << alcGetString(defaultDevice, ALC_DEVICE_SPECIFIER) << "\" as the default sound device.\n";

	globals.defaultDeviceName = defaultDevice_str;
	globals.currentAudioDevice = defaultDevice;

	auto vecPos = std::find(globals.audioDeviceNames.begin(), globals.audioDeviceNames.end(), defaultDevice_str);
	if (vecPos != globals.audioDeviceNames.end())
		std::iter_swap(globals.audioDeviceNames.begin() + 1, vecPos);

	globals.alctx = alcCreateContext(defaultDevice, nullptr);

	if (!alcMakeContextCurrent(globals.alctx)) {
		std::cerr << "Failed to set active context.\n";
	}

	//play_wav_immediate("./res/flup.wav", 1.f, 1.f, false, globals.alctx);
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
