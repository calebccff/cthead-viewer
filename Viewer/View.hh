#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "File.hh"
#include "Vars.h"

namespace ct {

	enum RenderType {
		CT_RENDER_SIMPLE = 0,
		CT_RENDER_DEPTH,
		CT_RENDER_VOLUME,
	};

	class View {
	  private:
		int slice_;
		std::shared_ptr<CTFile> file;
	
	  protected:
		View(std::shared_ptr<CTFile> file);

	  public:
		int width;
		int height;
		int slices;
		RenderType renderType = CT_RENDER_SIMPLE;
		void slice(int slice);
		int slice();

		virtual sf::Vector3i get_coord(int z, int y, int x) = 0;
		void doRender(sf::Texture* tex);

		std::array<double, 4> renderSimple(const sf::Vector3i coords);
		std::array<double, 4> renderTrigger(const sf::Vector3i coords);
		std::array<double, 4> renderTransferFunction(const sf::Vector3i coords);
	};

	class TopView : public View {
	  public:
		TopView(std::shared_ptr<CTFile> file);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};

	class SideView : public View {
	  public:
		SideView(std::shared_ptr<CTFile> file);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};

	class FrontView : public View {
	  public:
		FrontView(std::shared_ptr<CTFile> file);
		sf::Vector3i get_coord(int z, int y, int x) override;
	};
}
