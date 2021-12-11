#pragma once

#include "wnd_style.h"

BEGIN_NAMESPACE(WndDesign)


struct ScrollWndStyle : WndStyle {
	// The entire scrollable region. The size and position is relative to the parent size.
	// is_auto or is_center should not be not used.
	struct RegionStyle {
	public:
		LengthTag _width = 100pct;
		LengthTag _height = 100pct;
		LengthTag _left = 0pct;
		LengthTag _top = 0pct;
	public:
		constexpr void Set(LengthTag width, LengthTag height, LengthTag left = 0pct, LengthTag top = 0pct) {
			_width = width; _height = height; _left = left; _top = top;
		}
	} entire_region;
};

constexpr LengthTag length_tag_max = LengthTag(short(0x7FFF));
constexpr LengthTag position_min = LengthTag(-short(0x7FFF) / 2);
constexpr ScrollWndStyle::RegionStyle scroll_region_infinite = {
	length_tag_max, length_tag_max, position_min, position_min
};

enum class Positioning {
	Fixed,    // The child window will be floating on the top, not scrolled along with the base layer.  
			  // By calling Wnd::AddChild, you add a fixed window by default.

	Sticky,	  // The child window will be floating on the top, and can be scrolled along with the base 
			  //   layer when it's position falls in the parent window, but it will stay fixed at the 
			  //   edge if it is scrolled out.
			  // You must specify the boundary(padding) of the window, as well as the normal
			  //   position of the window on the base layer.

	Static,   // The child window is on the base layer that can be scrolled along. 
};


// Padding for sticky windows.
struct Padding {
public:
	LengthTag _left;
	LengthTag _top;
	LengthTag _right;
	LengthTag _down;
public:
	explicit constexpr Padding(LengthTag left, LengthTag top, LengthTag right, LengthTag down) :
		_left(left), _top(top), _right(right), _down(down) {
	}
	explicit constexpr Padding(LengthTag all = length_auto) :
		_left(all), _top(all), _right(all), _down(all) {
	}
	constexpr Padding& left(LengthTag left) { _left = left; return *this; }
	constexpr Padding& top(LengthTag top) { _top = top; return *this; }
	constexpr Padding& right(LengthTag right) { _right = right; return *this; }
	constexpr Padding& down(LengthTag down) { _down = down; return *this; }
	constexpr void SetAll(LengthTag all) { _left = _top = _right = _down = all; }
};

constexpr Padding padding_null = Padding();


END_NAMESPACE(WndDesign)
