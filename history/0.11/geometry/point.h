#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)

struct Point {
	int x;
	int y;
};


constexpr int int_min = 1 << 31;
constexpr Point minimum_point = { int_min, int_min };
constexpr Point origin_point = { 0, 0 };


END_NAMESPACE(WndDesign)