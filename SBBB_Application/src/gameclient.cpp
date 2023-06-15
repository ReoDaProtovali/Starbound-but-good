#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"

GameClient::GameClient() {
	stateManager.bindClientState(GameStateEnum::NO_STATE, (GameState*)&State_None);
	stateManager.bindClientState(GameStateEnum::IN_WORLD, (GameState*)&State_ClientWorld);
	stateManager.setState(GameStateEnum::IN_WORLD);
	gw.setVSync(true);
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
	m_stopping = true;
	clientThread.join();
}
void GameClient::run(SharedQueue<std::exception_ptr>& p_exceptionQueue) {
	Messenger<MouseEvent, int>& mouseMessenger = Messenger<MouseEvent, int>::Get(); // one-way messenger for capturing mouse events
	Subject<MouseEvent>& mouseSubject = globals.mouseSubject; // for re-transmitting mouse events to client

	Observer<KeyEvent> keyObserver{ globals.keySubject };
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

			gw.displayNewFrame();

			if (m_stopping) break;
		}
		gw.cleanUp();
	} catch (std::exception& ex) {
		ERROR_LOG("Exception in " << __FILE__ << " at " << __LINE__ << ": " << ex.what());
		p_exceptionQueue.push(std::current_exception());
		gw.cleanUp();
		return;
	}
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
