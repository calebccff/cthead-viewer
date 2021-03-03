#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>

namespace ct {

enum RenderType {
	CT_RENDER_SIMPLE = 0,
	CT_RENDER_DEPTH,
	CT_RENDER_VOLUME,
	CT_RENDER_3D,
};

struct CTFile;

struct Window {
	std::unique_ptr<sf::RenderWindow> window;
	std::shared_ptr<CTFile> file;

	RenderType renderType = CT_RENDER_SIMPLE;

	bool do_render = false;
	int frameCount = 0;

	// Field of View for 3D render
	// in degrees
	float fov = 40;
};

void renderQ2(ct::Window *w);
void renderQ3(ct::Window *w);

}