#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)

struct Size {
	uint width;
	uint height;
	
	bool operator==(const Size& size) const { return width == size.width && height == size.height; }
	bool operator!=(const Size& size) const { return !operator==(size); }
	bool IsEmpty() const {
		return width == 0 || height == 0;
	}

	uint Area() const { return width * height; }
};



constexpr uint length_max = 0x4FFFFFFF;  // In case for the conversion to int, or there may be overflow.
constexpr uint length_min = 0;
constexpr Size size_max = { length_max, length_max };
constexpr Size size_min = { length_min, length_min };



END_NAMESPACE(WndDesign)