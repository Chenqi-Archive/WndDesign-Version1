#pragma once

#include "../geometry/geometry.h"
#include "../figure/color.h"

#include "../common/exception.h"


BEGIN_NAMESPACE(WndDesign)

struct LengthTag {		// sizeof(LengthTag) == 4 bytes
	short length;
	bool is_auto : 1;
	bool is_percent : 1;
	bool is_center : 1;
	constexpr LengthTag operator-() const { return LengthTag(-length, is_auto, is_percent, is_center); }
public:
	explicit constexpr LengthTag(short length, bool is_auto = false, bool is_percent = false, bool is_center = false) :
		length(length), is_auto(is_auto), is_percent(is_percent), is_center(is_center) {
	}
	bool operator!=(const LengthTag& rhs) const {
		return length != rhs.length || is_auto != rhs.is_auto || is_percent != rhs.is_percent || is_center != rhs.is_center;
	}
	constexpr operator uchar() const {
		if (is_auto || is_percent || is_center || length < 0) { return 0; }
		return static_cast<uchar>(length);
	}
	constexpr operator ushort() const {
		if (is_auto || is_percent || is_center || length < 0) { return 0; }
		return static_cast<uchar>(length);
	}
};


#pragma warning(disable : 4455) // literal suffix identifiers that do not start with an underscore are reserved.
constexpr LengthTag operator""px(unsigned long long number) {
	return LengthTag(static_cast<short>(number), false, false, false);
}
constexpr LengthTag operator""pct(unsigned long long number) {
	return LengthTag(static_cast<short>(number), false, true, false);
}
#pragma warning(default : 4455) 

constexpr LengthTag px(int number) {
	return LengthTag(static_cast<short>(number), false, false, false);
}
constexpr LengthTag pct(int number) {
	return LengthTag(static_cast<short>(number), false, true, false);
}

constexpr LengthTag length_auto = LengthTag( 0, true, false, false);
constexpr LengthTag position_auto = length_auto;
constexpr LengthTag position_center = LengthTag( 0, false, false, true);


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

	// The hint for compositing.
	// These render hints have no effect on a system window.
	struct RenderStyle {	// 4 byte
	public:
		// Effect
		uchar _opacity = 0xFF;   // The opacity of all contents. 
		uchar _blur_radius = 0;  // The gaussian blur radius for overlapped content. Only useful when transparent.

		uchar _z_index = 0;       // The z-index. If z-index > 0, the window must have an independent layer.

		// Hint
		bool _position_change : 1;			// The window's position may change frequently.
		bool _width_change : 1;				// The window's size may change frequently.
		bool _height_change : 1;			
		bool _destroy : 1;					// The window may be removed or readded to the parent frequently.
		bool _redraw : 1;					// The window may redraw its contents frequently.
		bool _background_transparent : 1;	// The background may be transparent somewhere. 
											//   It will automatically be true if you have set the opacity less than 255.
			
		bool _mouse_penetrate_content : 1;    // Mouse penetrate for opaque regions that has non-255 opacity.
		bool _mouse_penetrate_background : 1; // Mouse penetrate for background regions that has non-255 opacity.

		bool IsOpaque() const { return _opacity == 0xFF && _blur_radius == 0 && _background_transparent == false; }
	
	public:
		constexpr RenderStyle() :_position_change(false), _width_change(false), _height_change(false),
			_destroy(false), _redraw(false), _background_transparent(false), 
			_mouse_penetrate_content(false), _mouse_penetrate_background(false) {}
		constexpr RenderStyle& opacity(uchar opacity) { _opacity = opacity; return *this; }
		constexpr RenderStyle& opacity(LengthTag opacity) { 
			if (opacity.is_percent) { _opacity = 0xFF * opacity.length / 100; }; return *this; 
		}
		constexpr RenderStyle& blur_radius(uchar blur_radius) { _blur_radius = blur_radius; return *this; }
		constexpr RenderStyle& z_index(uchar z_index) { _z_index = z_index; return *this; }
		constexpr RenderStyle& position_change(bool position_change) { _position_change = position_change; return *this; }
		constexpr RenderStyle& width_change(bool width_change) { _width_change = width_change; return *this;}
		constexpr RenderStyle& height_change(bool height_change) { _height_change = height_change; return *this;}
		constexpr RenderStyle& size_change(bool size_change) { _width_change = _height_change = size_change; return *this; }
		constexpr RenderStyle& destroy(bool destroy) { _destroy = destroy; return *this; }
		constexpr RenderStyle& redraw(bool redraw) { _redraw = redraw; return *this; }
		constexpr RenderStyle& background_transparent(bool background_transparent) { 
			_background_transparent = background_transparent; return *this; 
		}
		constexpr RenderStyle& mouse_penetrate_content(bool mouse_penetrate_content) {
			_mouse_penetrate_content = mouse_penetrate_content; return *this;
		}
		constexpr RenderStyle& mouse_penetrate_background(bool mouse_penetrate_background) {
			_mouse_penetrate_background = mouse_penetrate_background; return *this;
		}
	}render;


	////////////////////////////////////////////////////////////////////////////
	//// Styles used by the window itself: background, border, other styles ////
	////////////////////////////////////////////////////////////////////////////

	// Background.  // 4 bytes
	Color32 background = color_transparent;

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
};


END_NAMESPACE(WndDesign)
