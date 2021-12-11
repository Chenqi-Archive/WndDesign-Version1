#pragma once

#include "../common/core.h"
#include "point.h"
#include "size.h"

BEGIN_NAMESPACE(WndDesign)

struct Rect {
	Point point;
	Size size;

	// The corner points.
	Point LeftTop() const {
		return point;
	}
	Point LeftBottom() const {
		return { point.x, point.y + static_cast<int>(size.height)};
	}
	Point RightTop() const {
		return { point.x + static_cast<int>(size.width), point.y};
	}
	Point RightBottom() const {
		return { point.x + static_cast<int>(size.width), point.y + static_cast<int>(size.height)};
	}

	uint Area() const { return size.width * size.height; }

	bool Contains(const Rect& rect) const;
	bool Contains(const Point& point) const {
		return point >= LeftTop() && point < RightBottom();
	}

	Rect Intersect(const Rect& rect) const {
		Point posl1 = point, posh1 = RightBottom(),
			posl2 = rect.point, posh2 = rect.RightBottom();
		Point posl = { max(posl1.x, posl2.x), max(posl1.y, posl2.y) },
			posh = { min(posh1.x, posh2.x), min(posh1.y, posh2.y) };
		if (posh > posl) { 
			return { posl, {static_cast<uint>(posh.x - posl.x), static_cast<uint>(posh.y - posl.y) } };
		} else {
			return { posl, {} };
		}
	}
};

inline Rect operator+(const Rect& rect, const Vector& vector) {
	return { rect.point + vector, rect.size };
}
inline Rect operator-(const Rect& rect, const Vector& vector) {
	return { rect.point - vector, rect.size };
}

// Shrink the rect by size.
inline Rect operator-(const Rect& rect, Size size) {
	Rect new_rect = rect;
	if (new_rect.size.width >= size.width) {
		new_rect.size.width -= size.width;
	}
	if (new_rect.size.height >= size.height) {
		new_rect.size.height -= size.height;
	}
	return new_rect;
}


// If the point is inside the rect, return the point itself, if not, 
//   return a point in the rect that is nearest to the point.
inline Point FindNearestPoint(const Rect& rect, const Point& point) {

}
// If the smaller rect is inside the larger rect, return the rect itself, if not, 
//   return a rect in the larger rect that is nearest to the smaller rect.
inline Rect FintNearestRect(const Rect& larger_rect, const Rect& smaller_rect) {

}


constexpr Rect region_empty = { point_zero, size_min };
constexpr Rect region_infinite = { point_min, size_max };



END_NAMESPACE(WndDesign)