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
//#pragma omp parallel for
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
inline Eigen::Vector3f vectorFromAngle(float yaw, float pitch) {
	float x = cos(yaw);
    float z = sin(yaw);
    float y = tan(pitch);

	return Eigen::Vector3f {x, y, z}.normalized();
}

Eigen::Matrix<float,3,3> yawPitchRotMatrix(float yaw, float pitch) {
	Eigen::Matrix<float,3,3> yawMat, pitchMat;
	yawMat << cos(yaw), -sin(yaw), 0,
			  sin(yaw), cos(yaw),  0,
			  0,        0,         1;
	
	pitchMat << cos(pitch),    0, sin(pitch),
				0,           1, 0,
				-sin(pitch), 0, cos(pitch);
	
	return yawMat * pitchMat;
}

inline Eigen::Quaternionf
euler2Quaternion( const float roll,
                  const float pitch,
                  const float yaw )
{
    Eigen::AngleAxisf rollAngle(roll, Eigen::Vector3f::UnitX());
    Eigen::AngleAxisf pitchAngle(pitch, Eigen::Vector3f::UnitY());
    Eigen::AngleAxisf yawAngle(yaw, Eigen::Vector3f::UnitZ());

    Eigen::Quaternionf q = yawAngle * pitchAngle * rollAngle;
    return q;
}

std::array<double, 4> renderCube(const sf::Vector3i coords) {
	auto pix = std::array<double, 4>{0, 0, 0, 0};
	if (coords.x >= 90 && coords.x < 130
		&& coords.y >= 90 && coords.y < 130
		&& coords.z >= 50 && coords.z < 70) {
		pix[0] = 1;
		pix[1] = 1;
		pix[3] = 0;
	} else {
		pix[3] = 1;
	}
	return pix;
}

sf::Sprite RayView::doRender3() {
	auto fov = win->fov * M_PI/180.0;
	auto pixBuf = new sf::Uint8[width*height*4];
	const auto target = Eigen::Vector3f {width/2.0, height/2.0, slices/2.0};
	const auto camRadius = sqrt(height*height * 2);
	Eigen::Vector3f camDir;
	// camDir = euler2Quaternion(0, win->file->camRotY * M_PI/180.0,
	//  		win->file->camRotX * M_PI/180.0).vec().normalized();
	camDir = vectorFromAngle(win->file->camRotY * M_PI/180.0,
			win->file->camRotX * M_PI/180.0);
	const auto camFace = (target + camDir * camRadius);
	printf("Camera Pos: x: %f y: %f z: %f\n", camFace[0], camFace[1], camFace[2]);
	printf("Camera Dir: x: %f y: %f z: %f\n\n", camDir[0], camDir[1], camDir[2]);

#pragma omp parallel for
	for (size_t y = 0; y < height; y++)
	{
#pragma omp parallel for
		for (size_t x = 0; x < width; x++)
		{
			const auto L = win->file->lighting / 100.0 ;
			std::array<double, 4> pix{0, 0, 0, L};
//#pragma omp parallel for
			for(size_t z = 0; z < camRadius; z++) {
				const auto stepAngleX = map(x, 0, width, -fov/2.0, fov/2.0);
				const auto stepAngleY = map(y, 0, height, -fov/2.0, fov/2.0);

				// direction vector representing a single step of a ray
				Eigen::Vector3f step;
				step = euler2Quaternion(0, stepAngleY, stepAngleX) * camDir;
				step *= sqrt(x*x + y*y + z*z);

				//const Eigen::Quaternionf p = euler2Quaternion(0, stepAngleY, stepAngleX);
				// p.matrix() = yawPitchRotMatrix(stepAngleX, stepAngleY);

				// Eigen::Vector3f step = camDir * sqrt(x*x + y*y + z*z);
				// step.dot(p.vec());

				// Eigen::Vector3f step = (euler2Quaternion(0, stepAngleY, stepAngleX)
				// 	* camDir) * Q_rsqrt(x*x + y*y + z*z);

				// The rayposition is the edge of the arcball - the step direction * the depth
				Eigen::Vector3f rayPos = camFace - step;

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

					// auto slice_pix = renderCube(posNN);
					// pix = slice_pix;
				}/* else {
					break;
				}*/
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
