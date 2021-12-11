#pragma once

#include "value_tag.h"
#include "../figure/figure.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::shared_ptr;


struct WndStyle {

	// The size of the window.
	struct SizeStyle {
	public:
		struct SizeTag { ValueTag width; ValueTag height; };
		SizeTag _normal = { length_auto, length_auto };
		SizeTag _min = { px(length_min), px(length_min) };
		SizeTag _max = { px(length_max), px(length_max) };
	public:
		constexpr SizeStyle& normal(ValueTag width, ValueTag height) { _normal = { width, height }; return *this; }
		constexpr SizeStyle& min(ValueTag width, ValueTag height) { _min = { width, height }; return *this;}
		constexpr SizeStyle& max(ValueTag width, ValueTag height) { _max = { width, height }; return *this;}
		constexpr void setFixed(ValueTag width, ValueTag height) { _normal = _min = _max = { width, height }; }
	}size;


	// The margin between the outside of border and the parent window.
	struct PositionStyle {
	public:
		ValueTag _left = position_auto;
		ValueTag _top = position_auto;
		ValueTag _right = position_auto;
		ValueTag _down = position_auto;
	public:
		constexpr PositionStyle& left(ValueTag left) { _left = left; return *this; }
		constexpr PositionStyle& top(ValueTag top) { _top = top; return *this; }
		constexpr PositionStyle& right(ValueTag right) { _right = right; return *this; }
		constexpr PositionStyle& down(ValueTag down) { _down = down; return *this; }
		constexpr PositionStyle& setHorizontalCenter() { _left = _right = position_center; return *this; }
		constexpr PositionStyle& setVerticalCenter() { _top = _down = position_center; return *this; }
	}position;


	// The hint for compositing.
	struct RenderStyle {
	public:
		// Effect
		uchar _opacity = 0xFF;   // The opacity of all contents. 
		uchar _blur_radius = 0;  // The gaussian blur radius for overlapped bottom content. Only useful when transparent.

		uchar _z_index = 0;       // The z-index. If z-index > 0, the window must have an independent layer.

		// Hint
		bool _position_change : 1;			// The window's position may change frequently.
		bool _width_change : 1;				// The window's size may change frequently.
		bool _height_change : 1;			
		bool _destroy : 1;					// The window may be removed or readded to the parent frequently.
		bool _redraw : 1;					// The window may redraw its contents frequently.
		bool _background_transparent : 1;	// The background may be transparent somewhere. 
											//   It will automatically be true if you have set the opacity less than 255.
			
		bool _mouse_penetrate : 1;			// Mouse penetrate when opacity is less than 255.
		bool _mouse_penetrate_background : 1; // Mouse penetrate at background regions where opacity is less than 255.

		bool IsOpaque() const { return _opacity == 0xFF && _blur_radius == 0 && _background_transparent == false; }
	
	public:
		constexpr RenderStyle() :_position_change(false), _width_change(false), _height_change(false),
			_destroy(false), _redraw(false), _background_transparent(false), 
			_mouse_penetrate(false), _mouse_penetrate_background(false) {}
		constexpr RenderStyle& opacity(uchar opacity) { _opacity = opacity; return *this; }
		constexpr RenderStyle& opacity(ValueTag opacity) {  // You can use pct to specify the opacity.
			opacity.ConvertToPixel(0xFF); _opacity = static_cast<uchar>(opacity.AsUnsigned()); return *this;
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
		constexpr RenderStyle& mouse_penetrate(bool mouse_penetrate) {
			_mouse_penetrate = mouse_penetrate; return *this;
		}
		constexpr RenderStyle& mouse_penetrate_background(bool mouse_penetrate_background) {
			_mouse_penetrate_background = mouse_penetrate_background; return *this;
		}
	}render;


	// Border
	struct BorderStyle {    // 8 bytes
	public:
		ushort _width = 0; 
		ushort _radius = 0;	// Rounded angle radius.
		Color _color = ColorSet::Black;
	public:
		constexpr BorderStyle& width(ushort width) { _width = width; return *this; }
		constexpr BorderStyle& radius(ushort radius) { _radius = radius; return *this; }
		constexpr BorderStyle& color(Color color) { _color = color; return *this; }
	}border;


	// Background
	struct BackgroundStyle {



		void set(Alloc<Figure*> figure);
		void set(Color color){}
	}background;

};


END_NAMESPACE(WndDesign)
