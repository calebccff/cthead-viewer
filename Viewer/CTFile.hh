#pragma once

#include <vector>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Vars.h"
namespace ct {
	struct CTFile {
		short minBrightness;
		short maxBrightness;

		short pixMap[CT_IMAGE_SLICES][CT_IMAGE_HEIGHT][CT_IMAGE_WIDTH]; // z/x/y
	};

	CTFile* LoadFile(char* filename, std::unique_ptr<CTFile> const &data);
}