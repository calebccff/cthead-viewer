#include <vector>
#include <fstream>
#include <ios>
#include <iterator>
#include <climits>
#include <memory>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "File.hh"

#define TRANSFER_FUNCTION 1

namespace ct {
	CTFile* LoadFile(char* filename) {
		std::ifstream file;
		int i = 0, j = 0, k = 0;
		CTFile *data = (CTFile*)malloc(sizeof(CTFile));

		data->minBrightness = SHRT_MAX;
		data->maxBrightness = SHRT_MIN;

		file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
		file.open(filename, std::ios::binary | std::ios::in);

		for(k = 0; k < CT_IMAGE_SLICES; k++) {
			for(j = 0; j < CT_IMAGE_HEIGHT; j++) {
				for(i = 0; i < CT_IMAGE_WIDTH; i++) {
					sf::Int16 pixel;

					file.read((char* )&pixel, sizeof(short));
					if (pixel < data->minBrightness) data->minBrightness = pixel;
					if (pixel > data->maxBrightness) data->maxBrightness = pixel;

					data->pixMap[k][j][i] = pixel;
				}
			}
		}
		file.close();

		if (data->maxBrightness - data->minBrightness == 0)
			throw std::runtime_error("min and max brightness are the same");

		return data;
	}
}