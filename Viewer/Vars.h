#pragma once

constexpr int CT_IMAGE_WIDTH = 256;
constexpr int CT_IMAGE_HEIGHT = 256;
constexpr int CT_IMAGE_SLICES = 109; // this is the Z axis

enum CTViewType {
	TOP = 0,
	FRONT,
	SIDE,
};

inline float map(float val, float oldmin, float oldmax, float newmin, float newmax) {
	return (val - oldmin) / oldmax * newmax + newmin;
}