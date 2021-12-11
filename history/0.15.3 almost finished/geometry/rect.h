#pragma once

#include "../common/core.h"
#include "point.h"
#include "size.h"


BEGIN_NAMESPACE(WndDesign)


struct Rect {
	Point point;
	Size size;

	explicit constexpr Rect() :point(), size() {}
	explicit constexpr Rect(Point point, Size size) : point(point), size(size) {}
	explicit constexpr Rect(int x, int y, uint width, uint height) :point(x, y), size(width, height) {}
	
	const Point LeftTop() const { return point; }
	const Point LeftBottom() const { return Point(point.x, point.y + static_cast<int>(size.height)); }
	const Point RightTop() const {return Point(point.x + static_cast<int>(size.width), point.y);}
	const Point RightBottom() const {return Point(point.x + static_cast<int>(size.width), point.y + static_cast<int>(size.height));}
	const Point Center() const {return Point(point.x + static_cast<int>(size.width / 2), point.y + static_cast<int>(size.height / 2) );}
	
	bool IsEmpty() const { return size.IsEmpty(); }
	uint Area() const { return size.Area(); }

	bool operator==(const Rect& rect) const { return point == rect.point && size == rect.size; }
	bool operator!=(const Rect& rect) const { return point != rect.point || size != rect.size; }

	bool Contains(Point point) const { return point >= LeftTop() && point < RightBottom(); }
	bool Contains(const Rect& rect) const { return rect.point >= point && rect.RightBottom() <= RightBottom(); }

	const Rect Intersect(const Rect& rect) const {
		Point posl1 = point, posh1 = RightBottom(),
			posl2 = rect.point, posh2 = rect.RightBottom();
		Point posl = Point(max(posl1.x, posl2.x), max(posl1.y, posl2.y)),
			posh = Point(min(posh1.x, posh2.x), min(posh1.y, posh2.y));
		if (posh > posl) { 
			return Rect(posl, Size(static_cast<uint>(posh.x - posl.x), static_cast<uint>(posh.y - posl.y)));
		} else {
			return Rect(posl, size_min);
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
			Point posl = Point(min(posl1.x, posl2.x), min(posl1.y, posl2.y)),
				posh = Point(max(posh1.x, posh2.x), max(posh1.y, posh2.y));
			return Rect(posl, Size(uint(posh.x - posl.x), uint(posh.y - posl.y)));
		}
	}
};



constexpr Rect region_empty = Rect(point_zero, size_min);
constexpr Rect region_infinite = Rect(point_min, size_max);



inline const Rect operator+(const Rect& rect, const Vector& vector) {
	return Rect(rect.point + vector, rect.size);
}

inline const Rect operator-(const Rect& rect, const Vector& vector) {
	return Rect(rect.point - vector, rect.size);
}


END_NAMESPACE(WndDesign)