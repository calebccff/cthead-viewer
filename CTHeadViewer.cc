#include <vector>
#include <iostream>
#include <fstream>

#include "Vars.h"
#include "File.hh"
#include "View.hh"

#include "Window.hh"

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
	auto w = new ct::Window;

	if (argc == 1) {
		std::cout << "Please enter the path to CTHead" << std::endl;
		exit(2);
	}
	// Load the global pixel buf
	try {
		w->file = std::make_shared<ct::CTFile>(*ct::LoadFile(argv[1]));
	} catch (std::fstream::failure e) {
		std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
	} catch (std::runtime_error e) {
		std::cout << "Exception reading file " << e.what() << std::endl;
	}

	std::cout << "Min: " << w->file->minBrightness << " Max: " << w->file->maxBrightness << std::endl;

	w->renderType = ct::CT_RENDER_SIMPLE;

	w->window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1200, 800), "CTHead Viewer", sf::Style::Default, sf::ContextSettings(24));
	glEnable(GL_TEXTURE_2D);
	w->window->setFramerateLimit(240);
	ImGui::SFML::Init(*w->window);

	sf::Clock deltaClock;
	bool running = true;
	while (running) {
		sf::Event event;
		while (w->window->pollEvent(event)) {
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

		ImGui::SFML::Update(*w->window, deltaClock.restart());

		//window.clear(sf::Color::Black);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto ios = ImGui::GetIO();
		w->do_render = false;

		// ImGui::SetNextWindowPos(ImVec2(5, 400));
		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("FPS: %f", ios.Framerate);
		ImGui::Text("Render type");
		int rt = static_cast<int>(w->renderType);
		if (ImGui::RadioButton("Simple Slice Render", &rt, ct::CT_RENDER_SIMPLE)
		|| ImGui::RadioButton("Volume Render", &rt, ct::CT_RENDER_VOLUME)
		|| ImGui::RadioButton("Simple Depth Render", &rt, ct::CT_RENDER_DEPTH)
		|| ImGui::RadioButton("3D Render", &rt, ct::CT_RENDER_3D)) {
			w->renderType = static_cast<ct::RenderType>(rt);
			w->do_render = true;
		}
		switch(w->renderType) {
			case ct::CT_RENDER_3D:
				if (ImGui::SliderInt("Cam rot X", &w->file->camRotX, 0, 360))
					w->do_render = true;
				if (ImGui::SliderInt("Cam rot Y", &w->file->camRotY, 0, 360))
					w->do_render = true;
				if (ImGui::SliderFloat("Field of View", &w->fov, 0.0, 180.0))
					w->do_render = true;
			case ct::CT_RENDER_VOLUME:
				if (ImGui::SliderInt("Skin Opacity", &w->file->skin_opacity, 0, 100))
					w->do_render = true;
				if (ImGui::SliderInt("Light Brightness", &w->file->lighting, 0, 100))
					w->do_render = true;
				break;
			default:
				break;
		}
		ImGui::End();

		switch (w->renderType)
		{
		case ct::CT_RENDER_SIMPLE:
		case ct::CT_RENDER_VOLUME:
		case ct::CT_RENDER_DEPTH:
			ct::renderQ2(w);
			break;
		case ct::CT_RENDER_3D:
			ct::renderQ3(w);
			break;
		default:
			break;
		}

/*

TODO: 3D render with textures
https://www.codeproject.com/Articles/352270/Getting-Started-with-Volume-Rendering-using-OpenGL

== Render Pipeline
 - Generate 2D textures (based on simple render or TF)
 - For each slice, generate quads with the texture, support alpha

*/

		ImGui::SFML::Render(*w->window);
		w->window->display();

		w->frameCount++;
	}

	ImGui::SFML::Shutdown();
}
