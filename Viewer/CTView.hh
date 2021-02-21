#pragma once

#include <vector>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "RenderMaps.hh"
#include "CTFile.hh"
#include "Vars.h"

namespace ct {

	class CTView {
	  private:
		std::vector<sf::Uint8> pixBuf;
		ct::CTFile* file;
		sf::Color (*renderFunc)(sf::Int16 val);

	  public:
		struct CTView* getView(size_t slice = 0);
		void setRenderMap(const RenderMap map);

		CTView(ct::CTFile* file);
		~CTView();
	};
}
