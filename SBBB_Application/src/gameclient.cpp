#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"

GameClient::GameClient() {
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
	try {
		// important, opengl NEEDS this
		gw.bindToThisThread();
#ifndef DISABLE_DEBUG_STATS
		globals.debug.drawThread = std::this_thread::get_id();
#endif // DISABLE_DEBUG_STATS

		auto& res = ResourceManager::Get();
		auto& gs = GenericShaders::Get();

		while (true) {

			if (gw.hasChangedFullscreenState()) {
				resizeWindow(gw.windowWidth, gw.windowHeight);
			}
			if (flagResize) {
				flagResize = false;
				resizeWindow(newWidth, newHeight);
			}

			renderFPSGauge.update(0.5f);
			testInput();
			processDebugStats();
			render();

			if (m_stopping) break;
		}
		//gw.cleanUp();
	} catch (std::exception& ex) {
		ERROR_LOG("Exception in " << __FILE__ << " at " << __LINE__ << ": " << ex.what());
		p_exceptionQueue.push(std::current_exception());
		return;
	}
}

void GameClient::render()
{
	renderer.bindScreenFBOAsRenderTarget();
	renderer.setClearColor(glm::vec4(0.2f, 0.2f, 0.3f, 0.0f));
	renderer.clearScreen();

	int drawnChunkCount = renderer.drawWorld();
#ifndef DISABLE_DEBUG_STATS
	if (drawnChunkCount != 0) globals.debug.drawnChunkCount = drawnChunkCount;
#endif
	glEnable(GL_DEPTH_TEST);
	renderer.testDraw();

	gw.clear();
	gw.bind();
	glDisable(GL_DEPTH_TEST);
	renderer.drawLighting();
	gw.displayNewFrame();
}


void GameClient::resizeWindow(uint32_t p_w, uint32_t p_h)
{
	gw.windowWidth = p_w;
	gw.windowHeight = p_h;
	gw.setViewport(0, 0, p_w, p_h);
	renderer.setViewport(p_w, p_h);
	render();
}

void GameClient::testInput()
{
	static Observer<MouseEvent> mouseObserver;
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

	//if (inp.testKeyDown(SDLK_3)) {
	//	renderer.swapCameras();
	//}

	if (inp.testKeyDown(SDLK_5)) {
		gw.toggleFullscreen();
	}
	camVelocity *= 0.95;
	cam.pos += glm::vec3(camVelocity.x, camVelocity.y, 0.f);

	while (auto obs = mouseObserver.observe()) {
		MouseEvent m = obs.value();
		m.x = m.x / (float)gw.windowWidth;
		m.y = m.y / (float)gw.windowHeight;
		renderer.testButton.onUpdate(GUIEvent{ m, KeyEvent() });
	}
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
		db.screenW = renderer.screenWidth;
		db.screenH = renderer.screenHeight;
		db.windowW = renderer.windowWidth;
		db.windowH = renderer.windowHeight;
		db.statUpdate = true;
	}
#endif
}
