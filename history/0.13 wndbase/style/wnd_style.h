#pragma once

#include "length_tag.h"

#include "../geometry/geometry.h"
#include "../figure/color.h"

#include "../common/exception.h"


BEGIN_NAMESPACE(WndDesign)


// The style for painting inside the parent window.
// Parent window parses the style and provide canvases for child windows.

struct WndStyle {
	//////////////////////////////////////////////////////////////////////
	//// Styles used by parent windows: size, position, render styles ////
	//////////////////////////////////////////////////////////////////////
	
	// The size of the window.
	struct SizeStyle {   // 24 bytes
	public:
		struct SizeTag { LengthTag width; LengthTag height; };
		SizeTag _normal = { length_auto, length_auto };
		SizeTag _min = { 0px, 0px };
		SizeTag _max = { 0x7FFFpx, 0x7FFFpx };
	public:
		constexpr SizeStyle& normal(LengthTag width, LengthTag height) { _normal = { width, height }; return *this; }
		constexpr SizeStyle& min(LengthTag width, LengthTag height) { _min = { width, height }; return *this;}
		constexpr SizeStyle& max(LengthTag width, LengthTag height) { _max = { width, height }; return *this;}
		constexpr void SetFixed(LengthTag width, LengthTag height) { _normal = _min = _max = { width, height }; }
	}size;

	// The margin between the border and the parent window.
	struct PositionStyle {    // 16 bytes
	public:
		LengthTag _left = position_auto;
		LengthTag _top = position_auto;
		LengthTag _right = position_auto;
		LengthTag _down = position_auto;
	public:
		constexpr PositionStyle& left(LengthTag left) { _left = left; return *this; }
		constexpr PositionStyle& top(LengthTag top) { _top = top; return *this; }
		constexpr PositionStyle& right(LengthTag right) { _right = right; return *this; }
		constexpr PositionStyle& down(LengthTag down) { _down = down; return *this; }
		constexpr PositionStyle& SetHorizontalCenter() { _left = _right = position_center; return *this; }
		constexpr PositionStyle& SetVerticalCenter() { _top = _down = position_center; return *this; }
	}position;

	// The hint for rendering.
	// These render hints have no effect on a system window.
	struct RenderStyle {	   // 3 byte
	public:
		// Effect
		uchar _opacity = 0xFF;   // The opacity of all contents. If there are already transparent contents, use the smaller value.
		uchar _blur_radius = 0;   // The gaussian blur radius for overlapped content. Only useful when transparent.

		// Hint
		bool _position_change : 1;			// The window's position may change frequently.
		bool _width_change : 1;				// The window's size may change frequently.
		bool _height_change : 1;			
		bool _destroy : 1;					// The window may be removed or readded to the parent frequently.
		bool _redraw : 1;					// The window may redraw its contents frequently.
		bool _background_transparent : 1;	// The background may be transparent somewhere. 
											//   It will automatically be true if you have set the opacity less than 255.

		bool IsOpaque() const { return _opacity == 0xFF && _blur_radius == 0 && _background_transparent == false; }
	
	public:
		constexpr RenderStyle() :_position_change(false), _width_change(false), _height_change(false),
			_destroy(false), _redraw(false), _background_transparent(false) {}
		constexpr RenderStyle& opacity(uchar opacity) { _opacity = opacity; return *this; }
		constexpr RenderStyle& opacity(LengthTag opacity) { if (opacity.is_percent) { _opacity = 0xFF * opacity.length / 100; }; return *this; }
		constexpr RenderStyle& blur_radius(uchar blur_radius) { _blur_radius = blur_radius; return *this; }

		constexpr RenderStyle& position_change(bool position_change) { _position_change = position_change; return *this; }
		constexpr RenderStyle& width_change(bool width_change) { _width_change = width_change; return *this;}
		constexpr RenderStyle& height_change(bool height_change) { _height_change = height_change; return *this;}
		constexpr RenderStyle& size_change(bool size_change) { _width_change = _height_change = size_change; return *this; }
		constexpr RenderStyle& destroy(bool destroy) { _destroy = destroy; return *this; }
		constexpr RenderStyle& redraw(bool redraw) { _redraw = redraw; return *this; }
		constexpr RenderStyle& background_transparent(bool background_transparent) { _background_transparent = background_transparent; return *this; }
	}render;

	bool is_fixed : 1;

	// Mouse penetrate for opaque regions that has non-255 opacity.
	bool mouse_penetrate_content : 1;
	// Mouse penetrate for background regions that has non-255 opacity.
	bool mouse_penetrate_background : 1;


	////////////////////////////////////////////////////////////////////////////
	//// Styles used by the window itself: background, border, other styles ////
	////////////////////////////////////////////////////////////////////////////

	// Background. Supports solid color only.  // 4 bytes
	Color32 background;

	// Border
	struct BorderStyle {    // 8 bytes
	public:
		ushort _width = 0; 
		ushort _radius = 0;	// Rounded angle radius.
		Color32 _color = ColorSet::Black;
	public:
		constexpr BorderStyle& width(ushort width) { _width = width; return *this; }
		constexpr BorderStyle& radius(ushort radius) { _radius = radius; return *this; }
		constexpr BorderStyle& color(Color32 color) { _color = color; return *this; }
	}border;


	//// Shadow // Not supported.
	//struct ShadowStyle {
	//	char horizontal;
	//	char vertical;
	//	uchar blur;
	//} shadow;

	WndStyle() {
		is_fixed = false;
		mouse_penetrate_content = false;
		mouse_penetrate_background = true;
	}

	// Used for parent windows.
	Rect CalculateActualRegion(Size parent_size) const;
	bool ShouldAllocateNewLayer() const;
	bool MayRedraw() const;
};



inline void Percent2Pixel(LengthTag& length_tag, uint whole_length) {
	if (length_tag.is_percent) {
		length_tag.length = static_cast<short>(length_tag.length * static_cast<int>(whole_length) / 100);
		length_tag.is_percent = false;
	}
}

inline void BoundBetween(LengthTag& normal_length, LengthTag min_length, LengthTag max_length) {
	if (!min_length.is_auto && normal_length.length < min_length.length) {
		normal_length.length = min_length.length;
	}
	if (!max_length.is_auto && normal_length.length > max_length.length) {
		normal_length.length = max_length.length;
	}
}

inline Rect WndStyle::CalculateActualRegion(Size parent_size) const {
	Rect region;
	WndStyle::SizeStyle size = this->size;
	WndStyle::PositionStyle position = this->position;

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
			ExceptionDialog();
		}
		size._normal.width.length = position._right.length - position._left.length;
	}
	BoundBetween(size._normal.width, size._min.width, size._max.width);
	if (position._left.is_auto || position._left.is_center) {
		if (position._left.is_center) {
			position._left.length = (static_cast<short>(parent_size.width) - size._normal.width.length) / 2;
		} else if (!position._right.is_auto) {
			position._left.length = static_cast<short>(parent_size.width) - position._right.length - size._normal.width.length;
		} else {
			ExceptionDialog();
		}
	}

	//   For vertical axis.
	if (size._normal.height.is_auto) {
		if (position._top.is_auto || position._top.is_center || position._down.is_auto || position._down.is_center) {
			ExceptionDialog();
		}
		size._normal.height.length = position._down.length - position._top.length;
	} 	
	BoundBetween(size._normal.height, size._min.height, size._max.height);
	if (position._top.is_auto || position._top.is_center) {
		if (position._top.is_center) {
			position._top.length = (static_cast<short>(parent_size.height) - size._normal.height.length) / 2;
		} else if (!position._down.is_auto) {
			position._top.length = static_cast<short>(parent_size.height) - position._down.length - size._normal.height.length;
		} else {
			ExceptionDialog();
		}
	}
	

	// Step 3: Convert the LengthTag value to Rect region.
	region.size.width = size._normal.width.length > 0 ? size._normal.width.length : 0;
	region.size.height = size._normal.height.length > 0 ? size._normal.height.length : 0;
	region.point.x = position._left.length;
	region.point.y = position._top.length;
	return region;
}

inline bool WndStyle::ShouldAllocateNewLayer() const {
	if (render._position_change || render._width_change || render._height_change ||
		render._destroy || render._background_transparent) {
		return true;
	}
	if (render._opacity != 0xFF || render._blur_radius != 0) {
		return true;
	}
	return false;
}

inline bool WndStyle::MayRedraw() const {
	return render._redraw;
}



END_NAMESPACE(WndDesign)
