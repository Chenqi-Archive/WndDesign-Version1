#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


enum class BorderPosition {
	None = 0,
	Left = 10,			// HTLEFT              
	Top = 12,			// HTTOP               
	Right = 11,			// HTRIGHT             
	Bottom = 15,		// HTBOTTOM           
	LeftTop = 13,		// HTTOPLEFT           
	RightTop = 14,		// HTTOPRIGHT           
	LeftBottom = 16,	// HTBOTTOMLEFT        
	RightBottom = 17	// HTBOTTOMRIGHT       
};


inline BorderPosition PointToBorderPosition(Size window_size, uint border_width, Point point) {
	int x1 = static_cast<int>(border_width);
	int y1 = static_cast<int>(border_width);
	int x2 = static_cast<int>(window_size.width - border_width);
	int y2 = static_cast<int>(window_size.height - border_width);
	int width = static_cast<int>(window_size.width);
	int height = static_cast<int>(window_size.height);
	int x = point.x, y = point.y;
	if (x < 0) { return BorderPosition::None; }  //
	else if (x < x1) {
		if (y < 0) { return BorderPosition::None; }  //
		else if (y < y1) { return BorderPosition::LeftTop; }  //
		else if (y < y2) { return BorderPosition::Left; } //
		else if (y < height) { return BorderPosition::LeftBottom; } //
		else { return BorderPosition::None; }
	} else if (x < x2) {
		if (y < 0) { return BorderPosition::None; }  //
		else if (y < y1) { return BorderPosition::Top; }  //
		else if (y < y2) { return BorderPosition::None; } //
		else if (y < height) { return BorderPosition::Bottom; } //
		else { return BorderPosition::None; }
	} else if (x < width) {
		if (y < 0) { return BorderPosition::None; }  //
		else if (y < y1) { return BorderPosition::RightTop; }  //
		else if (y < y2) { return BorderPosition::Right; } //
		else if (y < height) { return BorderPosition::RightBottom; } //
		else { return BorderPosition::None; }
	} else {
		return BorderPosition::None;
	}
}


END_NAMESPACE(WndDesignHelper)