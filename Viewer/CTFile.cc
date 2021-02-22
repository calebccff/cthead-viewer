#include <vector>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <climits>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>

#include "CTFile.hh"

#define TRANSFER_FUNCTION 1

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

	// Map from the min/max of a given slice to a single byte
	sf::Color CTFile::renderSimple(sf::Int16 val) {
		if (maxBrightness - minBrightness == 0) maxBrightness++;
		sf::Uint8 pix = (val - minBrightness) * 255 / (maxBrightness - minBrightness);
		return sf::Color(pix, pix, pix, 0xFF);
	}

	sf::Color CTFile::renderTF(sf::Int16 val) {
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

	inline sf::Color CTFile::mapToPixel(CTView *view, int z, int y, int x) {
		sf::Int16 val;

		switch (view->type)
		{
		case FRONT:
			val = pixMap[y][z][x];
			break;
		case SIDE:
			val = pixMap[y][x][z];
			break;
		case TOP:
		default:
			val = pixMap[z][y][x];
			break;
		}
		switch (renderType) {
			case VOLREND:
				return renderTF(val);
			case SIMPLE:
			default:
				return renderSimple(val);
		}
	}

	// struct CTView* CTFile::getViewTop(size_t slice) {
	// 	struct CTView* view = new CTView;
	// 	sf::Int32 depth = CT_IMAGE_SLICES;

	// 	view->width = CT_IMAGE_WIDTH;
	// 	view->height = CT_IMAGE_HEIGHT;

	// 	for (size_t j = 0; j < view->height; j++)
	// 	{
	// 		for (size_t i = 0; i < view->width; i++)
	// 		{
	// 			sf::Color pix;
	// 			pix = mapToPixel(pixMap[slice][j][i]);

	// 			view->pixBuf.push_back(pix.r);
	// 			view->pixBuf.push_back(pix.g);
	// 			view->pixBuf.push_back(pix.b);
	// 			view->pixBuf.push_back(pix.a);
	// 		}
			
	// 	}

	// 	return view;
	// }

	// struct CTView* CTFile::getViewFront(size_t slice) {
	// 	struct CTView* view = new CTView;
	// 	sf::Int32 depth = CT_IMAGE_WIDTH;

	// 	view->width = CT_IMAGE_HEIGHT;
	// 	view->height = CT_IMAGE_SLICES;

	// 	for (size_t j = 0; j < view->height; j++)
	// 	{
	// 		for (size_t i = 0; i < view->width; i++)
	// 		{
	// 			sf::Color pix;
	// 			pix = mapToPixel(pixMap[j][slice][i]);

	// 			view->pixBuf.push_back(pix.r);
	// 			view->pixBuf.push_back(pix.g);
	// 			view->pixBuf.push_back(pix.b);
	// 			view->pixBuf.push_back(pix.a);
	// 		}
	// 	}

	// 	return view;
	// }

	// struct CTView* CTFile::getViewSide(size_t slice) {
	// 	struct CTView* view = new CTView;

	// 	view->width = CT_IMAGE_WIDTH;
	// 	view->height = CT_IMAGE_SLICES;

	// 	for (size_t j = 0; j < view->height; j++)
	// 	{
	// 		for (size_t i = 0; i < view->width; i++)
	// 		{
	// 			sf::Color pix;
	// 			pix = mapToPixel(pixMap[j][i][slice]);

	// 			view->pixBuf.push_back(pix.r);
	// 			view->pixBuf.push_back(pix.g);
	// 			view->pixBuf.push_back(pix.b);
	// 			view->pixBuf.push_back(pix.a);
	// 		}
	// 	}

	// 	return view;
	// }

	void CTFile::getView_(CTView* view, size_t slice) {

		if (renderType == VOLREND)
			slice = 0;

		// First slice also fills out the pixel buffer
		for (size_t j = 0; j < view->height; j++)
		{
			for (size_t i = 0; i < view->width; i++)
			{
				sf::Color pix;
				pix = mapToPixel(view, slice, j, i);

				view->pixBuf.push_back(pix.r);
				view->pixBuf.push_back(pix.g);
				view->pixBuf.push_back(pix.b);
				view->pixBuf.push_back(renderType == VOLREND ? 0x00 : pix.a);
			}
		}

		if (renderType == SIMPLE)
			return;

		//float aaccum[view->height][view->width];
		// We already did the first layer, start slice at 1
		for(size_t s = 1; s < view->depth; s++) {
			for (size_t j = 0; j < view->height; j++)
			{
				for (size_t i = 0; i < view->width; i++)
				{
					sf::Color pix;
					float L = 1;
					pix = mapToPixel(view, s, j, i);
					// if (pix.a = 0xFF)
					// 	continue;
					int off = j * i * 4; // Offset in pixBuf
					// if (view->pixBuf.at(off+3) == 0 || pix.a == 0)
					// 	continue;

					// sf::Color pCol(view->pixBuf.at(off+0), view->pixBuf.at(off+1),
					// 	view->pixBuf.at(off+2), view->pixBuf.at(off+3));
					
					view->pixBuf.at(off) += 
						((1.0-view->pixBuf.at(off+3)/255.0) * (1.0 - pix.a/255.0) * L * pix.r/255.0) * 255;
					view->pixBuf.at(off+1) += 
						((1.0-view->pixBuf.at(off+3)/255.0) * (1.0 - pix.a/255.0) * L * pix.g/255.0) * 255;
					view->pixBuf.at(off+2) += 
						((1.0-view->pixBuf.at(off+3)/255.0) * (1.0 - pix.a/255.0) * L * pix.b/255.0) * 255;
					
					// Calculating transparency, but we need this value to represent opacity
					// a^accum
					view->pixBuf.at(off+3) = ((1.0 - view->pixBuf.at(off+3)/255.0)
						* pix.a/255.0) * 255;
					
					// if (view->pixBuf.at(off) >100 || view->pixBuf.at(off+3) < 170) {
					// 	std::cout << "Interesting pixel" << pix.r << " " << pix.a << std::endl;
					// }
				}
				
			}
		}
		
	}

	struct CTView* CTFile::getView(const CTViewType type, size_t slice) {
		struct CTView* view = new CTView;
		view->type = type;

		switch (type)
		{
			case CTViewType::TOP:
				view->width = CT_IMAGE_WIDTH;
				view->height = CT_IMAGE_HEIGHT;
				view->depth = CT_IMAGE_SLICES;
				getView_(view, slice);
				break;
			case CTViewType::FRONT:
				view->width = CT_IMAGE_HEIGHT;
				view->height = CT_IMAGE_SLICES;
				view->depth = CT_IMAGE_HEIGHT;
				getView_(view, slice);
				break;
			case CTViewType::SIDE:
				view->width = CT_IMAGE_WIDTH;
				view->height = CT_IMAGE_SLICES;
				view->depth = CT_IMAGE_WIDTH;
				getView_(view, slice);
				break;
			default:
			break;
		}

		return view;
	}
}