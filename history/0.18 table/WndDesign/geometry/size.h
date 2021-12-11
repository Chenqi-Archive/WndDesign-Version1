#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)


struct Size {
	uint width;
	uint height;

	explicit constexpr Size() :width(0), height(0) {}
	explicit constexpr Size(uint width, uint height) : width(width), height(height) {}

	bool operator==(const Size& size) const { return width == size.width && height == size.height; }
	bool operator!=(const Size& size) const { return width != size.width || height != size.height;}
	bool IsEmpty() const { return width == 0 || height == 0; }
	uint Area() const { return width * height; }
};



constexpr uint length_max = 0x3FFFFFFF;  // For safe conversion to int.
constexpr uint length_min = 0;
constexpr Size size_max = Size(length_max, length_max);
constexpr Size size_min = Size(length_min, length_min);


END_NAMESPACE(WndDesign)