#pragma once

#include "../common/core.h"

#include "vector.h"
#include "size.h"


BEGIN_NAMESPACE(WndDesign)


struct Point {
	int x;
	int y;

	explicit constexpr Point() :x(0), y(0) {}
	explicit constexpr Point(int x, int y) : x(x), y(y) {}

	bool operator==(const Point& point) const { return x == point.x && y == point.y; }
	bool operator!=(const Point& point) const { return x != point.x || y != point.y; }
	bool operator>=(const Point& point) const { return x >= point.x && y >= point.y; }
	bool operator<=(const Point& point) const { return x <= point.x && y <= point.y; }
	bool operator>(const Point& point) const { return x > point.x && y > point.y; }
	bool operator<(const Point& point) const { return x < point.x&& y < point.y; }
};



constexpr int position_min = -static_cast<int>(length_max) / 2;
constexpr int position_max = static_cast<int>(length_max) / 2;
constexpr Point point_min = Point(position_min, position_min);
constexpr Point point_max = Point(position_max, position_max);
constexpr Point point_zero = Point(0, 0);



// begin_point + vector = end_point
inline const Point operator+(const Point& begin_point, const Vector& vector) {
	return Point(begin_point.x + vector.x, begin_point.y + vector.y);
}

// end_point - vector = begin_point
inline const Point operator-(const Point& end_point, const Vector& vector) {
	return Point(end_point.x - vector.x, end_point.y - vector.y);
}

// end_point - begin_point = vector
inline const Vector operator-(const Point& end_point, const Point& begin_point) {
	return Vector(end_point.x - begin_point.x, end_point.y - begin_point.y);
}


END_NAMESPACE(WndDesign)