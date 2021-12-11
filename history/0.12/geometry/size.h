#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)

struct Size {
	uint width;
	uint height;
};



constexpr uint length_auto = 0x80000000;
constexpr uint length_max = -1;
constexpr uint length_min = 0;
constexpr Size size_auto = { length_auto, length_auto };
constexpr Size size_max = { length_max, length_max };
constexpr Size size_min = { length_min, length_min };




END_NAMESPACE(WndDesign)