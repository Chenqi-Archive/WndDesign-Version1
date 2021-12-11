#pragma once

#include "textbox_style.h"
#include "style_helper.h"

BEGIN_NAMESPACE(WndDesign)

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


// Used for auto resizing.
inline short CalculateTextboxWidth(const TextBoxStyle& style, uint text_width) {
	const TextBoxStyle::PaddingStyle& padding = style.padding;
	if (padding._left.is_percent || padding._right.is_percent) { ExceptionDialog(style_parse_exception); }
	short border_width = static_cast<short>(style.border._width);
	short width = static_cast<short>(text_width) + padding._left.length + padding._right.length + border_width * 2;
	return width > 0 ? width : 0;
}

inline short CalculateTextboxHeight(const TextBoxStyle& style, uint text_height) {
	const TextBoxStyle::PaddingStyle& padding = style.padding;
	if (padding._top.is_percent || padding._down.is_percent) { ExceptionDialog(style_parse_exception); }
	short border_width = static_cast<short>(style.border._width);
	short height = static_cast<short>(text_height) + padding._top.length + padding._down.length + border_width * 2;
	return height > 0 ? height : 0;
}


END_NAMESPACE(WndDesign)