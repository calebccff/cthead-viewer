#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector3.hpp>

#include "View.hh"
#include "Vars.h"

namespace ct
{

// Map from the min/max of a given slice to a single byte
std::array<double, 4> View::renderSimple(const sf::Vector3i coords) {
	auto pix = std::array<double, 4>{0, 0, 0, 0};
	auto val = win->file->pixMap[coords.z][coords.y][coords.x];

	const auto brightness = (val - win->file->minBrightness) * 1.0
		/ (win->file->maxBrightness - win->file->minBrightness);
	pix[0] = brightness;
	pix[1] = brightness;
	pix[2] = brightness;
	return pix;
}

std::array<double, 4> View::renderTrigger(const sf::Vector3i coords) {
	auto pix = std::array<double, 4>{0, 0, 0, 0};
	auto val = win->file->pixMap[coords.z][coords.y][coords.x];

	const auto brightness = val > 400 ? 1.0 : 0.0;
	pix[0] = brightness;
	pix[1] = brightness;
	pix[2] = brightness;
	pix[3] = 1.0-brightness;
	return pix;
}

std::array<double, 4> View::renderTransferFunction(const sf::Vector3i coords) {
	auto pix = std::array<double, 4>{0, 0, 0, 0};
	auto val = win->file->pixMap[coords.z][coords.y][coords.x];

	if (val < -300) {
		pix[3] = 0;
	} else if (val <= 49) {
		pix[0] = 1.0;
		pix[1] = 0.79;
		pix[2] = 0.6;
		pix[3] = win->file->skin_opacity*1.0/255;
	} else if (val <= 299) {
		pix[3] = 0;
	} else if (val < 4096) {
		pix[0] = 1.0;
		pix[1] = 1.0;
		pix[2] = 1.0;
		pix[3] = 0.8;
	}
	return pix;
}
} // namespace ct
