#pragma once

#include "wnd_style.h"

BEGIN_NAMESPACE(WndDesign)


struct ScrollWndStyle : WndStyle {
	// The entire scrollable region. The size and position is relative to the parent size.
	// is_auto or is_center should not be not used.
	struct RegionStyle {
	public:
		ValueTag _left = 0pct;
		ValueTag _top = 0pct;
		ValueTag _width = 100pct;
		ValueTag _height = 100pct;
	public:
		constexpr void Set(ValueTag left, ValueTag top, ValueTag width, ValueTag height) {
			_left = left; _top = top; _width = width; _height = height;
		}
	} entire_region;
};


constexpr ScrollWndStyle::RegionStyle region_infinite_tag = {
	position_min_tag, position_min_tag, length_max_tag, length_max_tag
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
	ValueTag _left;
	ValueTag _top;
	ValueTag _right;
	ValueTag _down;
public:
	explicit constexpr Padding(ValueTag left, ValueTag top, ValueTag right, ValueTag down) :
		_left(left), _top(top), _right(right), _down(down) {
	}
	explicit constexpr Padding(ValueTag all = length_auto) :
		_left(all), _top(all), _right(all), _down(all) {
	}
	constexpr Padding& left(ValueTag left) { _left = left; return *this; }
	constexpr Padding& top(ValueTag top) { _top = top; return *this; }
	constexpr Padding& right(ValueTag right) { _right = right; return *this; }
	constexpr Padding& down(ValueTag down) { _down = down; return *this; }
	constexpr void SetAll(ValueTag all) { _left = _top = _right = _down = all; }
};

constexpr Padding padding_null = Padding();


END_NAMESPACE(WndDesign)
