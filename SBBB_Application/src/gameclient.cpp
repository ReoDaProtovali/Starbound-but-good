#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"

GameClient::GameClient() {
	gw.setVSync(true);
}
GameClient::~GameClient() {
	gw.cleanUp();
}

void GameClient::start() {
	// important
	gw.unbindFromThisThread();
	clientThread = std::jthread(&GameClient::run, this);
}

void GameClient::stop() {
	m_stopping = true;
	clientThread.join();
}
void GameClient::run() {
	// important, opengl NEEDS this
	gw.bindToThisThread();

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

		testInput();
		render();

		if (m_stopping) break;
	}
}

void GameClient::render()
{
	renderer.bindScreenFBOAsRenderTarget();
	renderer.setClearColor(glm::vec4(0.8f, 0.8f, 1.0f, 0.0f));
	renderer.clearScreen();

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

	if (inp.testKeyDown(SDLK_3)) {
		renderer.swapCameras();
	}
	if (inp.testKeyDown(SDLK_5)) {
		gw.toggleFullscreen();
	}
	camVelocity *= 0.95;
	cam.pos += glm::vec3(camVelocity.x, camVelocity.y, 0.f);
}
