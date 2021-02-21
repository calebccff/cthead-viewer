#include <vector>
#include <iostream>

#include "Vars.h"
#include "CTFile.hh"

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <SFML/OpenGL.hpp>

int main(int argc, char* argv[])
{
	ct::CTFile* CTFile;

	sf::Texture topViewTexture;
	sf::Sprite topViewSprite;

	sf::Texture frontViewTexture;
	sf::Sprite frontViewSprite;

	sf::Texture sideViewTexture;
	sf::Sprite sideViewSprite;

	int sliceTop = 1;
	int sliceFront = 1;
	int sliceSide = 1;

	if (argc == 1) {
		std::cout << "Please enter the path to CTHead" << std::endl;
		exit(2);
	}
	// Load the global pixel buf
	std::unique_ptr<ct::CTFile> file = std::unique_ptr<ct::CTFile>(ct::LoadFile(argv[1], file));
	if (!file) {
		std::cerr << "Failed to load file, does it exist?" << std::endl;
		exit(1);
	}

	sf::RenderWindow window(sf::VideoMode(1200, 800), "CTHead Viewer", sf::Style::Default, sf::ContextSettings(32));
	window.setFramerateLimit(240);
	ImGui::SFML::Init(window);

	sf::Clock deltaClock;
	bool running = true;
	while (running) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			switch (event.type)
			{
			case sf::Event::Closed:
				running = false;
				break;
			case sf::Event::KeyReleased:
				if (event.key.code == sf::Keyboard::Escape)
					running = false;
				break;
			case sf::Event::Resized:
				// adjust the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			default:
				break;
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		//window.clear(sf::Color::Black);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui::SetNextWindowPos(ImVec2(5, 300));
		ImGui::SetNextWindowSize(ImVec2(200, 80));
		ImGui::Begin("Top View");
		ImGui::SliderInt("Layer", &sliceTop, 1, CT_IMAGE_SLICES);
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(330, 150));
		ImGui::SetNextWindowSize(ImVec2(200, 80));
		ImGui::Begin("Front View");
		ImGui::SliderInt("Layer", &sliceFront, 1, CT_IMAGE_HEIGHT);
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(630, 150));
		ImGui::SetNextWindowSize(ImVec2(200, 80));
		ImGui::Begin("Side View");
		ImGui::SliderInt("Layer", &sliceSide, 1, CT_IMAGE_WIDTH);
		ImGui::End();

		// ct::CTView* topView = CTFile->getView(ct::CTViewType::TOP, sliceTop-1);

		// topViewTexture.create(topView->width, topView->height);
		// topViewTexture.update(&topView->pixBuf[0]);
		// topViewSprite.setTexture(topViewTexture);

		// ct::CTView* frontView = CTFile->getView(ct::CTViewType::FRONT, sliceFront-1);

		// frontViewTexture.create(frontView->width, frontView->height);
		// frontViewTexture.update(&frontView->pixBuf[0]);
		// frontViewSprite.setTexture(frontViewTexture);
		// frontViewSprite.setPosition(300, 0);

		// ct::CTView* sideView = CTFile->getView(ct::CTViewType::SIDE, sliceSide-1);

		// sideViewTexture.create(sideView->width, sideView->height);
		// sideViewTexture.update(&sideView->pixBuf[0]);
		// sideViewSprite.setTexture(sideViewTexture);
		// sideViewSprite.setPosition(600, 0);

		window.draw(topViewSprite);
		window.draw(frontViewSprite);
		window.draw(sideViewSprite);
		ImGui::SFML::Render(window);
		window.display();

		delete topView;
		delete frontView;
		delete sideView;
	}

	ImGui::SFML::Shutdown();
}
