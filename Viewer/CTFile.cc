#include <vector>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <climits>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "CTFile.hh"

#define VOL_RENDER 1

namespace ct {

	CTFile::CTFile(char* filename) {
		std::ifstream file;
		int i = 0, j = 0, k = 0;

		minBrightness = SHRT_MAX;
		maxBrightness = SHRT_MIN;

		std::cout << "Loading file: " << filename << std::endl;

		file.open(filename, std::ios::binary | std::ios::in);

		for(k = 0; k < CT_IMAGE_SLICES; k++) {
			for(j = 0; j < CT_IMAGE_HEIGHT; j++) {
				for(i = 0; i < CT_IMAGE_WIDTH; i++) {
					short pixel;

					file.read((char* )&pixel, sizeof(short));
					if (pixel < minBrightness) minBrightness = pixel;
					if (pixel > maxBrightness) maxBrightness = pixel;

					pixMap[k][j][i] = pixel;
				}
			}
		}
		file.close();

		int sizeZ = *(&pixMap + 1) - pixMap;
		int sizeY = *(&pixMap[0] + 1) - pixMap[0];
		int sizeX = *(&pixMap[0][0] + 1) - pixMap[0][0];

		std::cout << "z: " << sizeZ << " y: " << sizeY << " x: " << sizeX << std::endl;

		// struct CTView* view = new struct CTView;
		// view->slices = generateView(&pixBuf);
		// view->type = CTViewType::TOP;
		// views.push_back(view);


		// views.push_back(loadView(CTViewType::TOP));
		// views.push_back(loadView(CTViewType::FRONT));
		// views.push_back(loadView(CTViewType::SIDE));
	}

#ifndef VOL_RENDER
	// Map from the min/max of a given slice to a single byte
	inline sf::Color CTFile::mapToPixel(sf::Int16 val) {
		if (maxBrightness - minBrightness == 0) maxBrightness++;
		sf::Uint8 pix = (val - minBrightness) * 255 / (maxBrightness - minBrightness);
		return Color(pix, pix, pix, 0xFF);
	}
#else
	inline sf::Color CTFile::mapToPixel(sf::Int16 val) {
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
#endif

	struct CTView* CTFile::getViewTop(size_t slice) {
		struct CTView* view = new CTView;
		sf::Int32 depth = CT_IMAGE_SLICES;

		view->width = CT_IMAGE_WIDTH;
		view->height = CT_IMAGE_HEIGHT;

		for (size_t j = 0; j < view->height; j++)
		{
			for (size_t i = 0; i < view->width; i++)
			{
				sf::Color pix;
				pix = mapToPixel(pixMap[slice][j][i]);

				view->pixBuf.push_back(pix.r);
				view->pixBuf.push_back(pix.g);
				view->pixBuf.push_back(pix.b);
				view->pixBuf.push_back(pix.a);
			}
			
		}

		return view;
	}

	struct CTView* CTFile::getViewFront(size_t slice) {
		struct CTView* view = new CTView;
		sf::Int32 depth = CT_IMAGE_WIDTH;

		view->width = CT_IMAGE_HEIGHT;
		view->height = CT_IMAGE_SLICES;

		for (size_t j = 0; j < view->height; j++)
		{
			for (size_t i = 0; i < view->width; i++)
			{
				sf::Color pix;
				pix = mapToPixel(pixMap[j][slice][i]);

				view->pixBuf.push_back(pix.r);
				view->pixBuf.push_back(pix.g);
				view->pixBuf.push_back(pix.b);
				view->pixBuf.push_back(pix.a);
			}
		}

		return view;
	}

	struct CTView* CTFile::getViewSide(size_t slice) {
		struct CTView* view = new CTView;

		view->width = CT_IMAGE_WIDTH;
		view->height = CT_IMAGE_SLICES;

		for (size_t j = 0; j < view->height; j++)
		{
			for (size_t i = 0; i < view->width; i++)
			{
				sf::Color pix;
				pix = mapToPixel(pixMap[j][i][slice]);

				view->pixBuf.push_back(pix.r);
				view->pixBuf.push_back(pix.g);
				view->pixBuf.push_back(pix.b);
				view->pixBuf.push_back(pix.a);
			}
		}

		return view;
	}

	struct CTView* CTFile::getView(const CTViewType type, size_t slice) {
		size_t width, height, depth;

		switch (type)
		{
			case CTViewType::TOP:
				return getViewTop(slice);
				break;
			case CTViewType::FRONT:
				return getViewFront(slice);
				break;
			case CTViewType::SIDE:
				return getViewSide(slice);
				break;
			default:
			break;
		}		
	}
}