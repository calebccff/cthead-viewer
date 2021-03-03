#pragma once

#include <vector>
#include <memory>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Vars.h"
namespace ct {
	struct CTFile {
		short minBrightness;
		short maxBrightness;
		int lighting = 99;
		int skin_opacity = 30;

		int camRotX = 10;
		int camRotY = 10;

		short pixMap[CT_IMAGE_SLICES][CT_IMAGE_HEIGHT][CT_IMAGE_WIDTH]; // z/y/x
	};

	CTFile* LoadFile(char* filename);
}