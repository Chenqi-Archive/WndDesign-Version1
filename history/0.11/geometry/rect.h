#pragma once

#include "../common/core.h"
#include "point.h"
#include "size.h"

BEGIN_NAMESPACE(WndDesign)

struct Rect {
	Point point;
	Size size;
};


constexpr Rect empty_region = { origin_point, null_size };
constexpr Rect full_region = { minimum_point, infinite_size };



END_NAMESPACE(WndDesign)