#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "RenderMaps.hh"
#include "Vars.h"

namespace ct
{
// Map from the min/max of a given slice to a single byte
sf::Color simpleRender(sf::Int16 val) {
	if (maxBrightness - minBrightness == 0) maxBrightness++;
	sf::Uint8 pix = (val - minBrightness) * 255 / (maxBrightness - minBrightness);
	return sf::Color(pix, pix, pix, 0xFF);
}

sf::Color transferFunction(sf::Int16 val) {
	sf::Uint8 r = 0, g = 0, b = 0, a = 0xFF;

	if (val < -300) {
		a = 0;
	} else if (val <= 49) {
		r = 255;
		g = 202;
		b = 153;
		a = 204;
	} else if (val <= 299) {
		a = 0;
	} else if (val < 4096) {
		r = 255;
		g = 255;
		b = 255;
		a = 204;
	}
	return sf::Color(r, g, b, a);
}
} // namespace ct
