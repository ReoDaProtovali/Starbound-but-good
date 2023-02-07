#include "GameClient.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "ResourceManager.hpp"

GameClient::GameClient() {
	//gw.setVSync(true);
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
	res.loadTexID("./res/me.png", TextureID::ME_TEXTURE);
	Sprite TestSprite{ glm::vec3(0, 0, 0), Rect(0, 0, 2, 2) };
	TestSprite.attachTexture(res.getTexture(TextureID::ME_TEXTURE));


	while (true) {

		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "Client running! " << "\n";
		DrawStates d;
		gw.clear();
		TestSprite.draw(gw, d);
		gw.displayNewFrame();

		if (m_stopping) break;
	}
}