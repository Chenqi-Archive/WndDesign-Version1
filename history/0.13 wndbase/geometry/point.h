#pragma once

#include "../common/core.h"

#include "vector.h"
#include "size.h"

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
inline const Point operator+(const Point& begin_point, const Vector& vector) {
	return { begin_point.x + vector.x, begin_point.y + vector.y };
}
// end_point - vector = begin_point
inline const Point operator-(const Point& end_point, const Vector& vector) {
	return { end_point.x - vector.x, end_point.y - vector.y };
}
// end_point - begin_point = vector
inline const Vector operator-(const Point& end_point, const Point& begin_point) {
	return { end_point.x - begin_point.x, end_point.y - begin_point.y };
}





constexpr int _pos_min = -static_cast<int>(length_max) / 2;
constexpr Point point_min = { _pos_min, _pos_min };
constexpr Point point_zero = { 0, 0 };


END_NAMESPACE(WndDesign)