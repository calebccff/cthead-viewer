#include <vector>
#include <iostream>
#include "math.h"

#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "File.hh"
#include "View.hh"
#include "Vars.h"

namespace ct
{
View::View(Window *w) : win(w)
{
}

TopView::TopView(Window *w) : View(w)
{
	width = CT_IMAGE_WIDTH;
	height = CT_IMAGE_HEIGHT;
	slices = CT_IMAGE_SLICES;
}
SideView::SideView(Window *w) : View(w)
{
	width = CT_IMAGE_HEIGHT;
	height = CT_IMAGE_SLICES;
	slices = CT_IMAGE_HEIGHT;
}
FrontView::FrontView(Window *w) : View(w)
{
	width = CT_IMAGE_HEIGHT;
	height = CT_IMAGE_SLICES;
	slices = CT_IMAGE_HEIGHT;
}
RayView::RayView(Window *w) : View(w)
{
	width = CT_IMAGE_WIDTH;
	height = CT_IMAGE_HEIGHT;
	slices = CT_IMAGE_SLICES;
}

int* View::slice() {
	return &slice_;
}

sf::Sprite View::doRender() {
	auto pixBuf = new sf::Uint8[width*height*4];

#pragma omp parallel for
	for (size_t j = 0; j < height; j++)
	{
#pragma omp parallel for
		for (size_t i = 0; i < width; i++)
		{
			const auto L = win->file->lighting / 100.0 ;
			std::array<double, 4> pix{0, 0, 0, L};
			switch(win->renderType) {
				case CT_RENDER_VOLUME:
				case CT_RENDER_DEPTH:
#pragma omp parallel for
					for(size_t z = 0; z < slices; z++) {
						if (win->renderType == CT_RENDER_DEPTH) {
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
					pix = renderSimple(get_coord(*slice(), j, i));
					break;
			}

			pixBuf[i * 4 + j*width * 4 + 0] = 255 * pix[0];
			pixBuf[i * 4 + j*width * 4 + 1] = 255 * pix[1];
			pixBuf[i * 4 + j*width * 4 + 2] = 255 * pix[2];
			pixBuf[i * 4 + j*width * 4 + 3] = 255 * (1.0 - pix[3]);
		}
	}

	const auto tex = new sf::Texture;
	const auto spr = new sf::Sprite;
	tex->create(width, height);
	tex->update(pixBuf);
	spr->setTexture(*tex);
	return *spr;
}

// Calculate a direction vector based on the 2 angles
Eigen::Vector3f vectorFromAngle(float angX, float angY) {
	// float x = cos(angX);
    // float z = sin(angX);
    // float y = tan(angY);

	float x = sin(angX);
    float z = cos(angX);
    float y = tan(angY);

	return Eigen::Vector3f {x, y, z};
}

sf::Sprite RayView::doRender3(Eigen::Vector3f pos) {
	auto fov = win->fov * M_PI/180.0;
	auto pixBuf = new sf::Uint8[width*height*4];
	const auto target = Eigen::Vector3f {width/2.0, height/2.0, slices/2.0};
	const auto step = (target - pos).normalized();
#pragma omp parallel for
	for (size_t y = 0; y < height; y++)
	{
#pragma omp parallel for
		for (size_t x = 0; x < width; x++)
		{
			const auto L = win->file->lighting / 100.0 ;
			std::array<double, 4> pix{0, 0, 0, L};
#pragma omp parallel for
			for(size_t z = 0; z < slices; z++) {
				const auto stepAngleX = map(x, 0, width, 0, fov);
				const auto stepAngleY = map(y, 0, height, 0, fov);

				// temp is the step direction offset by the angle for this pixel
				const auto temp = (step + vectorFromAngle(stepAngleX, stepAngleY)).normalized();
				// We now cast our ray in the direction of temp, the distance we cast
				// is the magnitude of our current depth
				const auto rayPos = pos + temp * Eigen::Vector3f(x, y, z).norm();
				if (rayPos.x() >= 0 && rayPos.x() < width
				 && rayPos.y() >= 0 && rayPos.y() < height
				 && rayPos.z() >= 0 && rayPos.z() < slices) {
					 auto posNN = get_coord(round(rayPos.z()), round(rayPos.y()), round(rayPos.x()));
					auto slice_pix = renderTransferFunction(posNN);
					
					const auto t = pix[3];
					const auto t_slice = slice_pix[3];

					pix[0] += t * t_slice * L * slice_pix[0];
					pix[1] += t * t_slice * L * slice_pix[1];
					pix[2] += t * t_slice * L * slice_pix[2];
					pix[3] *= (L - t_slice*L);

					// auto slice_pix = renderTrigger(posNN);
					// if (slice_pix[3] < pix[3]) {
					// 	float depth = (z*1.0)/slices;
					// 	pix[0] = slice_pix[0]-depth;
					// 	pix[1] = slice_pix[1]-depth;
					// 	pix[2] = slice_pix[2]-depth;
					// 	pix[3] = 0;
					// }
				}
			}

			pixBuf[x * 4 + y*width * 4 + 0] = 255 * pix[0];
			pixBuf[x * 4 + y*width * 4 + 1] = 255 * pix[1];
			pixBuf[x * 4 + y*width * 4 + 2] = 255 * pix[2];
			pixBuf[x * 4 + y*width * 4 + 3] = 255 * (1.0 - pix[3]);

		}
	}
	
	const auto tex = new sf::Texture;
	const auto spr = new sf::Sprite;
	tex->create(width, height);
	tex->update(pixBuf);
	spr->setTexture(*tex);
	return *spr;
}

// posNN.x = posNN.x < 0 ? 0 : posNN.x;
// posNN.x = posNN.x > CT_IMAGE_WIDTH ? CT_IMAGE_WIDTH : posNN.x;
// posNN.y = posNN.y < 0 ? 0 : posNN.y;
// posNN.y = posNN.y > CT_IMAGE_HEIGHT ? CT_IMAGE_HEIGHT : posNN.y;
// posNN.z = posNN.z < 0 ? 0 : posNN.z;
// posNN.z = posNN.z > CT_IMAGE_SLICES ? CT_IMAGE_SLICES : posNN.z;

sf::Vector3i TopView::get_coord(int z, int y, int x) {
	return sf::Vector3i(x, y, z);
}

sf::Vector3i SideView::get_coord(int z, int y, int x) {
	return sf::Vector3i(z, x, y);
}

sf::Vector3i FrontView::get_coord(int z, int y, int x) {
	return sf::Vector3i(x, z, y);
}

sf::Vector3i RayView::get_coord(int z, int y, int x) {
	return sf::Vector3i(x, y, z);
}

} // namespace ct
