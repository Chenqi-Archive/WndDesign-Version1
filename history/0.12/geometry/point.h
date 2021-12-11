#pragma once

#include "../common/core.h"

#include "vector.h"

BEGIN_NAMESPACE(WndDesign)

struct Point {
	int x;
	int y;

	bool operator==(const Point& point) const {
		return x == point.x && y == point.y;
	}
	bool operator>=(const Point& point) const {
		return x >= point.x && y >= point.y;
	}
	bool operator<=(const Point& point) const {
		return x <= point.x && y <= point.y;
	}
	bool operator>(const Point& point) const {
		return x > point.x && y > point.y;
	}
	bool operator<(const Point& point) const {
		return x < point.x && y < point.y;
	}
};

// begin_point + vector = end_point
inline Point operator+(const Point& begin_point, const Vector& vector) {
	return { begin_point.x + vector.x, begin_point.y + vector.y };
}
// end_point - vector = begin_point
inline Point operator-(const Point& end_point, const Vector& vector) {
	return { end_point.x - vector.x, end_point.y - vector.y };
}
// end_point - begin_point = vector
inline Vector operator-(const Point& end_point, const Point& begin_point) {
	return { end_point.x - begin_point.x, end_point.y - begin_point.y };
}





constexpr int int_min = 1 << 31;
constexpr Point point_min = { int_min, int_min };
constexpr Point point_zero = { 0, 0 };


END_NAMESPACE(WndDesign)