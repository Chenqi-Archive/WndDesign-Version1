#pragma once

#include "wnd_style.h"
#include "scroll_wnd_style.h"
#include "../geometry/geometry_helper.h"
#include "../common/exception.h"
#include "../system/metrics.h"

BEGIN_NAMESPACE(WndDesign)


static const wstring style_parse_exception = L"Style Parse Error";


// Helper functions for calculating actual styles.


struct _Interval { 
	int position; uint length; 
	explicit _Interval(int position, uint length) :position(position), length(length) {} 
};

inline const Rect _Make_Rect_From_Interval(_Interval horizontal, _Interval vertical) {
	return Rect(horizontal.position, vertical.position, horizontal.length, vertical.length);
}


inline void BoundSizeBetween(ValueTag& normal_length, ValueTag min_length, ValueTag max_length) {
	if (!min_length.IsAuto() && normal_length.AsUnsigned() < min_length.AsUnsigned()) {
		normal_length.Set(min_length.AsUnsigned());
	}
	if (!max_length.IsAuto() && normal_length.AsUnsigned() > max_length.AsUnsigned()) {
		normal_length.Set(max_length.AsUnsigned());
	}
}

inline const Size CalculateMinSize(const WndStyle& style, Size parent_size) {
	WndStyle::SizeStyle::SizeTag min_size = style.size._min;
	min_size.width.ConvertToPixel(parent_size.width);
	min_size.height.ConvertToPixel(parent_size.height);
	return Size(min_size.width.AsUnsigned(), min_size.height.AsUnsigned());
}

inline const Size CalculateMaxSize(const WndStyle& style, Size parent_size) {
	WndStyle::SizeStyle::SizeTag max_size = style.size._max;
	max_size.width.ConvertToPixel(parent_size.width);
	max_size.height.ConvertToPixel(parent_size.height);
	return Size(max_size.width.AsUnsigned(), max_size.height.AsUnsigned());
}

inline _Interval CalculateActualWidth(const WndStyle& style, uint parent_width) {
	if (parent_width == 0) { return _Interval(0, 0); }

	WndStyle::SizeStyle size = style.size;
	WndStyle::PositionStyle position = style.position;

	size._normal.width.ConvertToPixel(parent_width);
	size._min.width.ConvertToPixel(parent_width);
	size._max.width.ConvertToPixel(parent_width);
	position._left.ConvertToPixel(parent_width);
	position._right.ConvertToPixel(parent_width);

	if (size._normal.width.IsAuto()) {
		if (position._left.IsAuto() || position._left.IsCenter() || position._right.IsAuto() || position._right.IsCenter()) {
			ExceptionDialog(style_parse_exception);
			size._normal.width.Set(parent_width);
		} else {
			size._normal.width.Set(position._right.AsSigned() - position._left.AsSigned());
		}
	}
	BoundSizeBetween(size._normal.width, size._min.width, size._max.width);

	if (position._left.IsAuto() || position._left.IsCenter()) {
		if (position._left.IsCenter()) {
			position._left.Set((static_cast<int>(parent_width) - size._normal.width.AsSigned()) / 2);
		} else if (!position._right.IsAuto()) {
			position._left.Set(static_cast<int>(parent_width) - position._right.AsSigned() - size._normal.width.AsSigned());
		} else {
			ExceptionDialog(style_parse_exception);
			position._left.Set(0);
		}
	}

	return _Interval(position._left.AsSigned(), size._normal.width.AsUnsigned());
}

inline _Interval CalculateActualHeight(const WndStyle& style, uint parent_height) {
	if (parent_height == 0) { return _Interval(0, 0); }

	WndStyle::SizeStyle size = style.size;
	WndStyle::PositionStyle position = style.position;

	size._normal.height.ConvertToPixel(parent_height);
	size._min.height.ConvertToPixel(parent_height);
	size._max.height.ConvertToPixel(parent_height);

	position._top.ConvertToPixel(parent_height);
	position._down.ConvertToPixel(parent_height);


	if (size._normal.height.IsAuto()) {
		if (position._top.IsAuto() || position._top.IsCenter() || position._down.IsAuto() || position._down.IsCenter()) {
			ExceptionDialog(style_parse_exception);
			size._normal.height.Set(parent_height);
		} else {
			size._normal.height.Set(position._down.AsSigned() - position._top.AsSigned());
		}
	}
	BoundSizeBetween(size._normal.height, size._min.height, size._max.height);

	if (position._top.IsAuto() || position._top.IsCenter()) {
		if (position._top.IsCenter()) {
			position._top.Set((static_cast<int>(parent_height) - size._normal.height.AsSigned()) / 2);
		} else if (!position._down.IsAuto()) {
			position._top.Set(static_cast<int>(parent_height) - position._down.AsSigned() - size._normal.height.AsSigned());
		} else {
			ExceptionDialog(style_parse_exception);
			position._top.Set(0);
		}
	}

	return _Interval(position._top.AsSigned(), size._normal.height.AsUnsigned());
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