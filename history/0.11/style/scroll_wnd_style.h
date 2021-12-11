#pragma once

#include "wnd_style.h"

BEGIN_NAMESPACE(WndDesign)

// If a window is scrollable, then all its subwindows are inside the scroll frame, 
//   except the scrollbar.


struct ScrollWndStyle : WndStyle{
	Size max_scroll_size = infinite_size;

};


enum class Position {
	// The position is relative to the background scrollable region (negative value allowed),
	//   and the window will be scrolled along with other scrolling window.
	Static,

	// The position is relative to the boundary of parent window, and not scrolled along.
	// For example, the scroll bar is fixed at the side.
	Fixed,

	// See below.
	Sticky,

	// Styles below in CSS standard are not used
	// Relative, 
	// Absolute, 
};

struct SubScrollWndStyle : WndStyle {
	// Only for the sub window of a scrollable window.
	Position position = Position::Static;

	// For sticky styles. 
	// The subwindow will always inside the bounded rectangle, and will stick to 
	//   the boundary if it is scrolled out as a Fixed window.
	// The left_top style in the WndStyle is the 
	ushort left = 10px;
	ushort top = 10px;
	ushort right = 10px;
	ushort bottom = 10px;
};


END_NAMESPACE(WndDesign)
