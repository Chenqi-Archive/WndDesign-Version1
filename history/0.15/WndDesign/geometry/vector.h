#pragma once

#include "../common/core.h"

BEGIN_NAMESPACE(WndDesign)

struct Vector {
	int x;
	int y;

	const Vector operator+(const Vector& vector) const {
		return { x + vector.x, y + vector.y };
	}

	const bool operator==(const Vector& vector) const {
		return x == vector.x && y == vector.y;
	}
};

constexpr Vector vector_zero = { 0, 0 };


END_NAMESPACE(WndDesign)