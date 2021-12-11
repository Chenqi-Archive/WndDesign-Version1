#pragma once

#include "../common/core.h"
#include "point.h"
#include "size.h"

BEGIN_NAMESPACE(WndDesign)

template<class T>
inline T max(T a, T b) { return a > b ? a : b; }
template<class T>
inline T min(T a, T b) { return a < b ? a : b; }


struct Rect {
	Point point;
	Size size;

	constexpr Rect() :point(point_zero), size(size_min) {}
	constexpr Rect(Point point, Size size) :point(point), size(size) {}
	
	// The corner points.
	const Point LeftTop() const {
		return point;
	}
	const Point LeftBottom() const {
		return { point.x, point.y + static_cast<int>(size.height)};
	}
	const Point RightTop() const {
		return { point.x + static_cast<int>(size.width), point.y};
	}
	const Point RightBottom() const {
		return { point.x + static_cast<int>(size.width), point.y + static_cast<int>(size.height)};
	}
	const Point Center() const {
		return { point.x + static_cast<int>(size.width / 2), point.y + static_cast<int>(size.height / 2) };
	}
	bool IsEmpty() const { return size.IsEmpty(); }
	uint Area() const { return size.Area(); }

	bool operator==(const Rect& rect)const {
		return point == rect.point && size == rect.size;
	}

	bool Contains(const Rect& rect) const {
		return rect.point >= point && rect.RightBottom() <= RightBottom();
	}
	bool Contains(const Point& point) const {
		return point >= LeftTop() && point < RightBottom();
	}

	const Rect Intersect(const Rect& rect) const {
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

	const Rect Union(const Rect& rect) const {
		if (IsEmpty()) { 
			return rect; 
		} else if (rect.IsEmpty()) {
			return *this;
		} else {
			Point posl1 = point, posh1 = RightBottom(),
				posl2 = rect.point, posh2 = rect.RightBottom();
			Point posl = { min(posl1.x, posl2.x), min(posl1.y, posl2.y) },
				posh = { max(posh1.x, posh2.x), max(posh1.y, posh2.y) };
			return { posl, {uint(posh.x - posl.x), uint(posh.y - posl.y)} };
		}
	}

	// Shrink the rect region by length.
	const Rect Shrink(uint length) const {
		uint max_length = min(size.width, size.height) / 2;
		if (length > max_length) { length = max_length; }
		return { {point.x + static_cast<int>(length), point.y + static_cast<int>(length)},
			{size.width - 2 * length, size.height - 2 * length} };
	}
};

inline const Rect operator+(const Rect& rect, const Vector& vector) {
	return { rect.point + vector, rect.size };
}
inline const Rect operator-(const Rect& rect, const Vector& vector) {
	return { rect.point - vector, rect.size };
}

// Shrink the rect by size.
inline const Rect operator-(const Rect& rect, Size size) {
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
inline const Point FindNearestPoint(const Rect& rect, const Point& point) {

}
// If the smaller rect is inside the larger rect, return the rect itself, if not, 
//   return a rect in the larger rect that is nearest to the smaller rect.
inline const Rect FintNearestRect(const Rect& larger_rect, const Rect& smaller_rect) {

}


constexpr Rect region_empty = { point_zero, size_min };
constexpr Rect region_infinite = { point_min, size_max };



END_NAMESPACE(WndDesign)