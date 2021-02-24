#include <vector>
#include <iostream>
#include <tgmath.h>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "File.hh"
#include "View.hh"
#include "Vars.h"

#define PI 3.14159265

namespace ct
{
	View::View(std::shared_ptr<CTFile> file) : file(file)
	{
		renderType = CT_RENDER_SIMPLE;
	}

	// View::~View() {
	// }

	TopView::TopView(std::shared_ptr<CTFile> file) : View(file)
	{
		width = CT_IMAGE_WIDTH;
		height = CT_IMAGE_HEIGHT;
		slices = CT_IMAGE_SLICES;
	}
	SideView::SideView(std::shared_ptr<CTFile> file) : View(file)
	{
		width = CT_IMAGE_HEIGHT;
		height = CT_IMAGE_SLICES;
		slices = CT_IMAGE_HEIGHT;
	}
	FrontView::FrontView(std::shared_ptr<CTFile> file) : View(file)
	{
		width = CT_IMAGE_HEIGHT;
		height = CT_IMAGE_SLICES;
		slices = CT_IMAGE_HEIGHT;
	}

	void View::slice(int slice) {
		if (slice > slices)
			return;
		slice_ = slice-1;
	}

	int View::slice() {
		return slice_;
	}

	void View::doRender(sf::Texture* tex) {
		auto pixBuf = new sf::Uint8[width*height*4];

#pragma omp parallel for
		for (size_t j = 0; j < height; j++)
		{
#pragma omp parallel for
			for (size_t i = 0; i < width; i++)
			{
				const auto L = file->lighting / 100.0 ;
				std::array<double, 4> pix{0, 0, 0, L};
				switch(renderType) {
					case CT_RENDER_VOLUME:
					case CT_RENDER_DEPTH:
#pragma omp parallel for
						for(size_t z = 0; z < slices; z++) {
							if (renderType == CT_RENDER_DEPTH) {
								auto slice_pix = renderTrigger(get_coord(z, j, i));
								if (slice_pix[3] < pix[3]) {
									float depth = (z*1.0)/slices;
									pix[0] = slice_pix[0]-depth;
									pix[1] = slice_pix[1]-depth;
									pix[2] = slice_pix[2]-depth;
									pix[3] = 0;
								}
							} else {
								auto slice_pix = renderTransferFunction(get_coord(z, j, i));
								const auto t = pix[3];
								const auto t_slice = slice_pix[3];

								pix[0] += t * t_slice * L * slice_pix[0];
								pix[1] += t * t_slice * L * slice_pix[1];
								pix[2] += t * t_slice * L * slice_pix[2];
								pix[3] *= (L - t_slice*L);
							}
						}
						break;
					case CT_RENDER_SIMPLE:
					default:
						pix = renderSimple(get_coord(slice(), j, i));
						break;
				}

				pixBuf[i*4+j*width*4 + 0] = 255 * pix[0];
				pixBuf[i*4+j*width*4 + 1] = 255 * pix[1];
				pixBuf[i*4+j*width*4 + 2] = 255 * pix[2];
				pixBuf[i*4+j*width*4 + 3] = 255 * (1.0 - pix[3]);
			}
		}

		tex->create(width, height);
		tex->update(pixBuf);
	}

	sf::Vector3i TopView::get_coord(int z, int y, int x) {
		return sf::Vector3i(x, y, z);
	}

	sf::Vector3i SideView::get_coord(int z, int y, int x) {
		return sf::Vector3i(z, x, y);
	}

	sf::Vector3i FrontView::get_coord(int z, int y, int x) {
		return sf::Vector3i(x, z, y);
	}

} // namespace ct
