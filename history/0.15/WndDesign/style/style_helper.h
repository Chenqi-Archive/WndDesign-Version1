#pragma once

#include "style.h"
#include "../system/metrics.h"

BEGIN_NAMESPACE(WndDesign)


// Helper functions for calculating actual styles.

inline void Percent2Pixel(LengthTag& length_tag, uint whole_length) {
	if (length_tag.is_percent) {
		length_tag.length = static_cast<short>(length_tag.length * static_cast<int>(whole_length) / 100);
		length_tag.is_percent = false;
	}
}

inline void BoundSizeBetween(LengthTag& normal_length, LengthTag min_length, LengthTag max_length) {
	if (!min_length.is_auto && normal_length.length < min_length.length) {
		normal_length.length = min_length.length;
	}
	if (!max_length.is_auto && normal_length.length > max_length.length) {
		normal_length.length = max_length.length;
	}
}

// Parent size is for calculating relative size, parent region is for caculating relative position. 
// For Wnd and Desktop, parent_region == {point_zero, parent_size}.
// For ScrollWnd, a static or sticky child's parent_region is the base layer's entire region, 
//   but parent_size is the size of the parent window.
inline Rect CalculateActualRegion(const WndStyle& style, Rect parent_region) {
	static const wstring exception = L"Style Parse Error";
	static const Size max_parent_size = GetDesktopSize();

	Size parent_size = parent_region.size;
	if (parent_size.IsEmpty()) { return region_empty; }
	if (parent_size.width >= max_parent_size.width) { parent_size.width = max_parent_size.width; }
	if (parent_size.height >= max_parent_size.height) { parent_size.height = max_parent_size.height; }


	Rect region;
	WndStyle::SizeStyle size = style.size;
	WndStyle::PositionStyle position = style.position;

	// Step 1: Convert all relative values to absolute values.
	Percent2Pixel(size._normal.width, parent_size.width);
	Percent2Pixel(size._normal.height, parent_size.height);
	Percent2Pixel(size._min.width, parent_size.width);
	Percent2Pixel(size._min.height, parent_size.height);
	Percent2Pixel(size._max.width, parent_size.width);
	Percent2Pixel(size._max.height, parent_size.height);

	Percent2Pixel(position._left, parent_size.width);
	Percent2Pixel(position._top, parent_size.height);
	Percent2Pixel(position._right, parent_size.width);
	Percent2Pixel(position._down, parent_size.height);

	// Step 2: Deduce the auto values.
	//   For horizontal axis.
	if (size._normal.width.is_auto) {
		if (position._left.is_auto || position._left.is_center || position._right.is_auto || position._right.is_center) {
			ExceptionDialog(exception);
			size._normal.width.length = parent_size.width;
		} else {
			size._normal.width.length = position._right.length - position._left.length;
		}
	}
	BoundSizeBetween(size._normal.width, size._min.width, size._max.width);
	if (position._left.is_auto || position._left.is_center) {
		if (position._left.is_center) {
			position._left.length = (static_cast<short>(parent_size.width) - size._normal.width.length) / 2;
		} else if (!position._right.is_auto) {
			position._left.length = static_cast<short>(parent_size.width) - position._right.length - size._normal.width.length;
		} else {
			ExceptionDialog(exception);
			position._left.length = 0;
		}
	}

	//   For vertical axis.
	if (size._normal.height.is_auto) {
		if (position._top.is_auto || position._top.is_center || position._down.is_auto || position._down.is_center) {
			ExceptionDialog(exception);
			size._normal.height.length = parent_size.height;
		} else {
			size._normal.height.length = position._down.length - position._top.length;
		}
	}
	BoundSizeBetween(size._normal.height, size._min.height, size._max.height);
	if (position._top.is_auto || position._top.is_center) {
		if (position._top.is_center) {
			position._top.length = (static_cast<short>(parent_size.height) - size._normal.height.length) / 2;
		} else if (!position._down.is_auto) {
			position._top.length = static_cast<short>(parent_size.height) - position._down.length - size._normal.height.length;
		} else {
			ExceptionDialog(exception);
			position._top.length = 0;
		}
	}

	// Step 3: Convert the LengthTag value to Rect region.
	region.size.width = size._normal.width.length > 0 ? size._normal.width.length : 0;
	region.size.height = size._normal.height.length > 0 ? size._normal.height.length : 0;
	region.point.x = position._left.length + parent_region.point.x;
	region.point.y = position._top.length + parent_region.point.y;
	return region;
}

// For normal Wnd and Desktop.
inline Rect CalculateActualRegion(const WndStyle& style, Size parent_size) {
	return CalculateActualRegion(style, { point_zero, parent_size });
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


inline Rect CalculateTextRegion(const TextBoxStyle& style, Size canvas_size) {
	Rect region;
	TextBoxStyle::PaddingStyle padding = style.padding;
	int border_width = static_cast<int>(style.border._width);
	Percent2Pixel(padding._left, canvas_size.width);
	Percent2Pixel(padding._top, canvas_size.height);
	Percent2Pixel(padding._right, canvas_size.width);
	Percent2Pixel(padding._down, canvas_size.height);
	int left = border_width + padding._left.length;
	int right = static_cast<int>(canvas_size.width) - border_width - padding._right.length;
	int top = border_width + padding._top.length;
	int down = static_cast<int>(canvas_size.height) - border_width - padding._down.length;
	if (right < left) { right = left; }
	if (down < top) { down = top; }
	return { {left, top}, {static_cast<uint>(right - left), static_cast<uint>(down - top)} };
}


inline Rect CalculateAccessibleRegion(ScrollWndStyle::RegionStyle entire_region, Size parent_size) {
	// Convert to actual pixels.
	Percent2Pixel(entire_region._width, parent_size.width);
	Percent2Pixel(entire_region._height, parent_size.height);
	Percent2Pixel(entire_region._left, parent_size.width);
	Percent2Pixel(entire_region._top, parent_size.height);
	// Make sure the accessible region overlaps parent region.
	Rect region;
	region.point.x = entire_region._left.length > 0 ? 0 : entire_region._left.length;
	region.point.y = entire_region._top.length > 0 ? 0 : entire_region._top.length;
	int width = region.point.x + entire_region._width.length < static_cast<int>(parent_size.width) ?
		static_cast<int>(parent_size.width) - region.point.x : entire_region._width.length;
	int height = region.point.y + entire_region._height.length < static_cast<int>(parent_size.height) ?
		static_cast<int>(parent_size.height) - region.point.y : entire_region._height.length;
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
	return {
		BoundInInterval(point.x, rect.point.x, rect.size.width),
		BoundInInterval(point.y, rect.point.y, rect.size.height)
	};
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
	int x1 = padding._left.is_auto ? _pos_min : padding._left.length;
	int x2 = padding._right.is_auto ? _pos_max : static_cast<int>(size.width) - padding._right.length;
	int y1 = padding._top.is_auto ? _pos_min : padding._top.length;
	int y2 = padding._down.is_auto ? _pos_max : static_cast<int>(size.height) - padding._down.length;
	if (x2 < x1) { x2 = x1; }
	if (y2 < y1) { y2 = y1; }
	return { {x1, y1}, {static_cast<uint>(x2 - x1), static_cast<uint>(y2 - y1)} };
}


END_NAMESPACE(WndDesign);