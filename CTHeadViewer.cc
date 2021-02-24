#include <vector>
#include <iostream>
#include <fstream>

#include "Vars.h"
#include "File.hh"
#include "View.hh"

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
	std::shared_ptr<ct::CTFile> file;

	auto topViewTexture = new sf::Texture;
	auto topViewSprite = new sf::Sprite;

	auto frontViewTexture = new sf::Texture;
	auto frontViewSprite = new sf::Sprite;

	auto sideViewTexture = new sf::Texture;
	auto sideViewSprite = new sf::Sprite;

	int sliceTop = 1;
	int sliceFront = 1;
	int sliceSide = 1;
	int renderType = ct::CT_RENDER_SIMPLE;

	if (argc == 1) {
		std::cout << "Please enter the path to CTHead" << std::endl;
		exit(2);
	}
	// Load the global pixel buf
	try {
		file = std::make_shared<ct::CTFile>(*ct::LoadFile(argv[1]));
	} catch (std::runtime_error e) {
		std::cout << "Exception reading file " << e.what() << std::endl;
	} catch (std::fstream::failure e) {
		std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
	}

	ct::TopView topView(file);
	ct::SideView sideView(file);
	ct::FrontView frontView(file);

	sf::RenderWindow window(sf::VideoMode(1200, 800), "CTHead Viewer", sf::Style::Default, sf::ContextSettings(24));
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

		bool do_render = false;
		auto ios = ImGui::GetIO();

		// ImGui::SetNextWindowPos(ImVec2(5, 400));
		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("FPS: %f", ios.Framerate);
		ImGui::Text("Render type");
		if (ImGui::RadioButton("Simple Slice Render", &renderType, ct::CT_RENDER_SIMPLE)
		|| ImGui::RadioButton("Volume Render", &renderType, ct::CT_RENDER_VOLUME)
		|| ImGui::RadioButton("Simple Depth Render", &renderType, ct::CT_RENDER_DEPTH)) {
			topView.renderType = static_cast<ct::RenderType>(renderType);
			sideView.renderType = static_cast<ct::RenderType>(renderType);
			frontView.renderType = static_cast<ct::RenderType>(renderType);
			do_render = true;
		}
		if (renderType == ct::CT_RENDER_VOLUME) {
			if (ImGui::SliderInt("Skin Opacity", &file->skin_opacity, 0, 100)
			|| ImGui::SliderInt("Light Brightness", &file->lighting, 0, 100)) {
				do_render = true;
			}
		}
		ImGui::End();

		if (topView.renderType == ct::CT_RENDER_SIMPLE) {
			// ImGui::SetNextWindowPos(ImVec2(5, 300));
			// ImGui::SetNextWindowSize(ImVec2(200, 80));
			ImGui::Begin("Top View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::SliderInt("Slice", &sliceTop, 1, CT_IMAGE_SLICES))
				do_render = true;
			ImGui::End();
		}

		if (sideView.renderType == ct::CT_RENDER_SIMPLE) {
			// ImGui::SetNextWindowPos(ImVec2(330, 150));
			// ImGui::SetNextWindowSize(ImVec2(200, 80));
			ImGui::Begin("Side View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::SliderInt("Slice", &sliceSide, 1, CT_IMAGE_WIDTH))
				do_render = true;
			ImGui::End();
		}

		if (frontView.renderType == ct::CT_RENDER_SIMPLE) {
			// ImGui::SetNextWindowPos(ImVec2(630, 150));
			// ImGui::SetNextWindowSize(ImVec2(200, 80));
			ImGui::Begin("Front View", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::SliderInt("Slice", &sliceFront, 1, CT_IMAGE_HEIGHT))
				do_render = true;
			ImGui::End();
		}

		if (do_render) {
			topView.slice(sliceTop);
			topView.doRender(topViewTexture);
			topViewSprite->setTexture(*topViewTexture);
			topViewSprite->setPosition(30, 30);
			

			sideView.slice(sliceSide);
			sideView.doRender(sideViewTexture);
			sideViewSprite->setTexture(*sideViewTexture);
			sideViewSprite->setPosition(330, 30);

			frontView.slice(sliceFront);
			frontView.doRender(frontViewTexture);
			frontViewSprite->setTexture(*frontViewTexture);
			frontViewSprite->setPosition(660, 30);
		}

		window.draw(*topViewSprite);
		window.draw(*frontViewSprite);
		window.draw(*sideViewSprite);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
