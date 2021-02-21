#pragma once

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "CTFile.hh"
#include "CTView.hh"
#include "Vars.h"

namespace ct
{
	enum RenderMap {
		CT_MAP_SIMPLE_RENDER = 0,
		CT_MAP_TRANSFER_FUNCTION,
		CT_MAP_VOLUME_RENDER,
		CT_MAP_MAX_INTENSITY,
	};

	sf::Color renderSimple(sf::Int16 val);
	sf::Color renderTransferFunction(sf::Int16 val);
} // namespace ct
