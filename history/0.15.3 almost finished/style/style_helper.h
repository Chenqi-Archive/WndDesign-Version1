#pragma once

#include "wnd_style.h"
#include "scroll_wnd_style.h"
#include "../common/exception.h"
#include "../system/metrics.h"

BEGIN_NAMESPACE(WndDesign)


static const wstring style_parse_exception = L"Style Parse Error";
static const Size max_parent_size = GetDesktopSize();


// Helper functions for calculating actual styles.

inline void BoundSizeBetween(ValueTag& normal_length, ValueTag min_length, ValueTag max_length) {
	if (!min_length.IsAuto() && normal_length.AsUnsigned() < min_length.AsUnsigned()) {
		normal_length.Set(min_length.AsUnsigned());
	}
	if (!max_length.IsAuto() && normal_length.AsUnsigned() > max_length.AsUnsigned()) {
		normal_length.Set(max_length.AsUnsigned());
	}
}

// Parent size is for calculating relative size, parent region is for caculating relative position. 
// For Wnd and Desktop, parent_region == {point_zero, parent_size}.
// For ScrollWnd, a static or sticky child's parent_region is the base layer's entire region.
inline Rect CalculateActualRegion(const WndStyle& style, Size parent_size) {
	if (parent_size.IsEmpty()) { return region_empty; }
	if (parent_size.width >= max_parent_size.width) { parent_size.width = max_parent_size.width; }
	if (parent_size.height >= max_parent_size.height) { parent_size.height = max_parent_size.height; }

	Rect region;
	WndStyle::SizeStyle size = style.size;
	WndStyle::PositionStyle position = style.position;

	// Step 1: Convert all relative values to absolute values.
	size._normal.width.ConvertToPixel(parent_size.width);
	size._normal.height.ConvertToPixel(parent_size.height);
	size._min.width.ConvertToPixel(parent_size.width);
	size._min.height.ConvertToPixel(parent_size.height);
	size._max.width.ConvertToPixel(parent_size.width);
	size._max.height.ConvertToPixel(parent_size.height);

	position._left.ConvertToPixel(parent_size.width);
	position._top.ConvertToPixel(parent_size.height);
	position._right.ConvertToPixel(parent_size.width);
	position._down.ConvertToPixel(parent_size.height);

	// Step 2: Deduce the auto values.
	//   For horizontal axis.
	if (size._normal.width.IsAuto()) {
		if (position._left.IsAuto() || position._left.IsCenter() || position._right.IsAuto() || position._right.IsCenter()) {
			ExceptionDialog(style_parse_exception);
			size._normal.width.Set(parent_size.width);
		} else {
			size._normal.width.Set(position._right.AsSigned() - position._left.AsSigned());
		}
	}
	BoundSizeBetween(size._normal.width, size._min.width, size._max.width);
	if (position._left.IsAuto() || position._left.IsCenter()) {
		if (position._left.IsCenter()) {
			position._left.Set((static_cast<int>(parent_size.width) - size._normal.width.AsSigned()) / 2);
		} else if (!position._right.IsAuto()) {
			position._left.Set(static_cast<int>(parent_size.width) - position._right.AsSigned() - size._normal.width.AsSigned());
		} else {
			ExceptionDialog(style_parse_exception);
			position._left.Set(0);
		}
	}

	//   For vertical axis.
	if (size._normal.height.IsAuto()) {
		if (position._top.IsAuto() || position._top.IsCenter() || position._down.IsAuto() || position._down.IsCenter()) {
			ExceptionDialog(style_parse_exception);
			size._normal.height.Set(parent_size.height);
		} else {
			size._normal.height.Set(position._down.AsSigned() - position._top.AsSigned());
		}
	}
	BoundSizeBetween(size._normal.height, size._min.height, size._max.height);
	if (position._top.IsAuto() || position._top.IsCenter()) {
		if (position._top.IsCenter()) {
			position._top.Set((static_cast<int>(parent_size.height) - size._normal.height.AsSigned()) / 2);
		} else if (!position._down.IsAuto()) {
			position._top.Set(static_cast<int>(parent_size.height) - position._down.AsSigned() - size._normal.height.AsSigned());
		} else {
			ExceptionDialog(style_parse_exception);
			position._top.Set(0);
		}
	}

	// Step 3: Convert the ValueTag value to Rect region.
	region.size.width = size._normal.width.AsUnsigned();
	region.size.height = size._normal.height.AsUnsigned();
	region.point.x = position._left.AsSigned();
	region.point.y = position._top.AsSigned();

	return region;
}

inline bool HasCompositeEffect(const WndStyle& style) {
	const WndStyle::RenderStyle& render = style.render;
	if (render._position_change || render._width_change || render._height_change ||
		render._destroy || render._background_transparent) {
		return true;
	}
	if (render._opacity != 0xFF || render._blur_radius != 0) {
		return true;
	}
	if (style.border._radius > 0) {
		return true;
	}
	return false;
}

inline bool MayRedraw(const WndStyle& style) {
	return style.render._redraw;
}



// Calculate the accessible region for scrollwnd's base layer according to region style.
inline Rect CalculateAccessibleRegion(ScrollWndStyle::RegionStyle entire_region, Size parent_size) {
	// Convert to actual pixels.
	entire_region._width.ConvertToPixel(parent_size.width);
	entire_region._height.ConvertToPixel(parent_size.height);
	entire_region._left.ConvertToPixel(parent_size.width);
	entire_region._top.ConvertToPixel(parent_size.height);
	// Make sure the accessible region overlaps parent region.
	Rect region;
	region.point.x = entire_region._left.AsSigned() > 0 ? 0 : entire_region._left.AsSigned();
	region.point.y = entire_region._top.AsSigned() > 0 ? 0 : entire_region._top.AsSigned();
	int width = region.point.x + entire_region._width.AsSigned() < static_cast<int>(parent_size.width) ?
		static_cast<int>(parent_size.width) - region.point.x : entire_region._width.AsSigned();
	int height = region.point.y + entire_region._height.AsSigned() < static_cast<int>(parent_size.height) ?
		static_cast<int>(parent_size.height) - region.point.y : entire_region._height.AsSigned();
	region.size.width = static_cast<uint>(width);
	region.size.height = static_cast<uint>(height);
	return region;
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

// For scrollwnd, calculate the bounding region for layer that is sticky.
inline Rect CalculateBoundingRegion(Padding padding, Size size) {
	int x1 = padding._left.IsAuto() ? position_min : padding._left.AsSigned();
	int x2 = padding._right.IsAuto() ? position_max : static_cast<int>(size.width) - padding._right.AsSigned();
	int y1 = padding._top.IsAuto() ? position_min : padding._top.AsSigned();
	int y2 = padding._down.IsAuto() ? position_max : static_cast<int>(size.height) - padding._down.AsSigned();
	if (x2 < x1) { x2 = x1; }
	if (y2 < y1) { y2 = y1; }
	return Rect(Point(x1, y1), Size(static_cast<uint>(x2 - x1), static_cast<uint>(y2 - y1)));
}



END_NAMESPACE(WndDesign);