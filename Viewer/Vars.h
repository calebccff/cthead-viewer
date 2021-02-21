#pragma once

constexpr int CT_IMAGE_WIDTH = 256;
constexpr int CT_IMAGE_HEIGHT = 256;
constexpr int CT_IMAGE_SLICES = 113; // this is the Z axis

enum CTViewType {
	TOP = 0,
	FRONT,
	SIDE,
};