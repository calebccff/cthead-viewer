#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <Eigen/Geometry>

#include "File.hh"
#include "Vars.h"
#include "../Window/Window.hh"

namespace ct {

	class View {
	  private:
		int slice_ = 1;
	
	  protected:
		View(Window *w);

	  public:
		int width;
		int height;
		int slices;
		Window *win;
		void slice(int slice);
		int* slice();

		virtual sf::Vector3i get_coord(int z, int y, int x) = 0;
		sf::Sprite doRender();

		std::array<double, 4> renderSimple(const sf::Vector3i coords);
		std::array<double, 4> renderTrigger(const sf::Vector3i coords);
		std::array<double, 4> renderTransferFunction(const sf::Vector3i coords);
	};

	class TopView : public View {
	  public:
		TopView(Window *w);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};

	class SideView : public View {
	  public:
		SideView(Window *w);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};

	class FrontView : public View {
	  public:
		FrontView(Window *w);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};

	class RayView : public View {
	  public:
		RayView(Window *w);
		sf::Vector3i get_coord(int z, int y, int x) override;
		sf::Sprite doRender3(Eigen::Vector3f pos);
	};
}
