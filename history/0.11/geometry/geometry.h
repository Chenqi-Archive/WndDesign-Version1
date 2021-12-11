#pragma once

#include "point.h"
#include "vector.h"
#include "size.h"
#include "rect.h"

BEGIN_NAMESPACE(WndDesign)

constexpr uint operator ""px(unsigned long long length) {
	return static_cast<uint>(length);
}

constexpr ushort operator ""deg(unsigned long long degree) {
	return static_cast<uint>(degree % 360);
}


END_NAMESPACE(WndDesign)