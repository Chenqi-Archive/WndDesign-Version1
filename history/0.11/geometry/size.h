#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)

struct Size {
	uint width;
	uint height;
};



constexpr Size infinite_size = { -1, -1 };
constexpr Size null_size = { 0, 0 };



END_NAMESPACE(WndDesign)