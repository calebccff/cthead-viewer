
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include <Eigen/Geometry>

#include "Window.hh"

#include "../Viewer/View.hh"

#define CT_IMGUIFLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize

namespace ct
{

inline bool _viewImGui(ct::View *view, std::string name, int nSlice) {
	int *slice;
	bool do_render;
	ImGui::Begin(name.c_str(), nullptr, CT_IMGUIFLAGS);
	slice = view->slice();
	if (ImGui::SliderInt("Slice", slice, 1, nSlice))
		do_render = true;
	ImGui::End();

	return do_render;
}

void renderQ2(ct::Window *w)
{
	static std::array<sf::Sprite, 3> sprites;
	static auto topView = new ct::TopView(w);
	static auto sideView = new ct::SideView(w);
	static auto frontView = new ct::FrontView(w);
	w->do_render = w->frameCount == 1 ? true : w->do_render;

	if (w->renderType == ct::CT_RENDER_SIMPLE) {
		w->do_render = _viewImGui(topView, "Top View", CT_IMAGE_SLICES) | w->do_render;
		w->do_render = _viewImGui(sideView, "Side View", CT_IMAGE_WIDTH) | w->do_render;
		w->do_render = _viewImGui(frontView, "Front View", CT_IMAGE_HEIGHT) | w->do_render;
	}

	if (w->do_render) {
		sprites =
		{
			topView->doRender(),
			sideView->doRender(),
			frontView->doRender(),
		};
	}

	for (size_t i = 0; i < sprites.size(); i++)
	{
		sprites[i].setPosition(30 + 300*i, 30);
		w->window->draw(sprites[i]);
	}
}

// Ray casting
void renderQ3(ct::Window *w)
{
	static auto rayView = new ct::RayView(w);
	static sf::Sprite sprite;

	if (w->do_render) {
		sprite = rayView->doRender3();
		sprite.setPosition(30, 30);
	}
	w->window->draw(sprite);
}

} // namespace ct