#pragma once

#include "geometry.h"

#include <cmath>


BEGIN_NAMESPACE(WndDesign)


inline uint square(int x) { return static_cast<uint>(x * x); }

inline float Distance(const Point& a, const Point& b) {
	return static_cast<float>(sqrt(square(a.x - b.x) + square(a.y - b.y)));
}

inline float Distance(const Point& point, const Rect& rect) {
	int x_min = rect.point.x;
	int x_max = rect.point.x + static_cast<int>(rect.size.width) - 1;
	int y_min = rect.point.y;
	int y_max = rect.point.y + static_cast<int>(rect.size.height) - 1;
	int x = point.x;
	int y = point.y;
	if (x < x_min) {
		if (y < y_min) {
			return Distance(point, Point{ x_min, y_min });
		} else if (y <= y_max) {
			return static_cast<float>(x_min - x);
		} else {
			return Distance(point, Point{ x_min, y_max });
		}
	} else if (x <= x_max) {
		if (y < y_min) {
			return static_cast<float>(y_min - y);
		} else if (y <= y_max) {
			return 0.0;
		} else {
			return static_cast<float>(y - y_max);
		}
	} else {
		if (y < y_min) {
			return Distance(point, Point{ x_max, y_min });
		} else if (y <= y_max) {
			return static_cast<float>(x - x_max);
		} else {
			return Distance(point, Point{ x_max, y_max });
		}
	}
}


// For bounding the current point in the accessible region.
inline int BoundInInterval(int val, int begin, uint length) {
	if (val < begin) { return begin; }
	int end = begin + length - 1;
	if (val > end) { return end; }
	return val;
}

inline Point BoundPointInRegion(Point point, Rect rect) {
	return Point(
		BoundInInterval(point.x, rect.point.x, rect.size.width),
		BoundInInterval(point.y, rect.point.y, rect.size.height)
	);
}

// Remind that the returned region includes the right bottom edge.
inline const Rect ShrinkRegionBySize(const Rect& region, Size size) {
	Rect new_rect;
	new_rect.point = region.point;
	new_rect.size.width = (region.size.width > size.width ? region.size.width - size.width : 0) + 1;
	new_rect.size.height = (region.size.height > size.height ? region.size.height - size.height : 0) + 1;
	return new_rect;
}

inline Rect BoundRectInRegion(Rect rect, const Rect& region) {
	// If the rect is bigger than the region, shrink the rect.
	if (rect.size.width > region.size.width) { rect.size.width = region.size.width; }
	if (rect.size.height > region.size.height) { rect.size.height = region.size.height; }
	// Calculate the bounding region for the left top point.
	rect.point = BoundPointInRegion(rect.point, ShrinkRegionBySize(region, rect.size));
	return rect;
}

// Shrink a rect region by length.
inline Rect ShrinkRegionByLength(const Rect& rect, uint length) {
	uint max_length = min(rect.size.width, rect.size.height) / 2;
	if (length > max_length) { length = max_length; }
	Rect region = rect;
	region.point.x += static_cast<int>(length);
	region.point.y += static_cast<int>(length);
	region.size.width -= 2 * length;
	region.size.height -= 2 * length;
	return region;
}


// For hit testing whether a point falls in the rounded rectangle.
inline bool PointInRoundedRectangle(const Rect& rect, uint radius, Point point) {
	if (!rect.Contains(point)) { return false; }
	uint max_radius = min(rect.size.width, rect.size.height) / 2;
	if (radius > max_radius) { radius = max_radius; }
	if (radius == 0) { return true; }
	int x1 = rect.point.x + static_cast<int>(radius);
	int	x2 = rect.point.x + static_cast<int>(rect.size.width - radius);
	int	y1 = rect.point.y + static_cast<int>(radius);
	int	y2 = rect.point.y + static_cast<int>(rect.size.height - radius);
	int x = point.x;
	int y = point.y;
	if (x < x1 && y < y1) { return square(x - x1) + square(y - y1) <= square(radius) ? true : false; }
	if (x < x1 && y > y2) { return square(x - x1) + square(y - y2) <= square(radius) ? true : false; }
	if (x > x2 && y < y1) { return square(x - x2) + square(y - y1) <= square(radius) ? true : false; }
	if (x > x2 && y > y2) { return square(x - x2) + square(y - y2) <= square(radius) ? true : false; }
	return true;
}



END_NAMESPACE(WndDesign)