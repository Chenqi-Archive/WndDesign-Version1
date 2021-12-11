#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct Vector {
	int x;
	int y;

	explicit constexpr Vector() :x(0), y(0) {}
	explicit constexpr Vector(int x, int y) :x(x), y(y) {}

	const Vector operator+(const Vector& vector) const { return Vector(x + vector.x, y + vector.y); }
	const Vector operator-(const Vector& vector) const { return Vector(x - vector.x, y - vector.y); }
	const bool operator==(const Vector& vector) const { return x == vector.x && y == vector.y; }
	const bool operator!=(const Vector& vector) const { return x != vector.x || y != vector.y; }
};



constexpr Vector vector_zero = Vector(0, 0);


END_NAMESPACE(WndDesign)