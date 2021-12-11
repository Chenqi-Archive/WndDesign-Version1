#pragma once

#include "textbox_style.h"
#include "style_helper.h"


BEGIN_NAMESPACE(WndDesign)


inline Rect CalculateTextRegion(const TextBoxStyle& style, Size canvas_size) {
	Rect region;
	TextBoxStyle::PaddingStyle padding = style.padding;
	int border_width = static_cast<int>(style.border._width);

	padding._left.ConvertToPixel(canvas_size.width);
	padding._top.ConvertToPixel(canvas_size.height);
	padding._right.ConvertToPixel(canvas_size.width);
	padding._down.ConvertToPixel(canvas_size.height);
	int left = border_width + padding._left.AsSigned();
	int right = static_cast<int>(canvas_size.width) - border_width - padding._right.AsSigned();
	int top = border_width + padding._top.AsSigned();
	int down = static_cast<int>(canvas_size.height) - border_width - padding._down.AsSigned();
	if (right < left) { right = left; }
	if (down < top) { down = top; }
	return Rect(Point(left, top), Size(static_cast<uint>(right - left), static_cast<uint>(down - top)));
}


// Used for auto resizing.
inline uint CalculateMaxTextWidth(const TextBoxStyle& style, uint parent_width) {
	ValueTag max_width_tag = style.size._max.width;
	max_width_tag.ConvertToPixel(parent_width);

	const TextBoxStyle::PaddingStyle& padding = style.padding;
	if (padding._left.IsPercent() || padding._right.IsPercent()) { ExceptionDialog(style_parse_exception); }
	int border_width = static_cast<int>(style.border._width);

	int text_width = max_width_tag.AsSigned() - (padding._left.AsSigned() + padding._right.AsSigned() + border_width * 2);
	return text_width > 0 ? static_cast<uint>(text_width) : 0;
}

inline uint CalculateTextboxWidth(const TextBoxStyle& style, uint text_width) {
	const TextBoxStyle::PaddingStyle& padding = style.padding;
	if (padding._left.IsPercent() || padding._right.IsPercent()) { ExceptionDialog(style_parse_exception); }
	int border_width = static_cast<int>(style.border._width);
	int width = static_cast<int>(text_width) + padding._left.AsSigned() + padding._right.AsSigned() + border_width * 2;
	return width > 0 ? static_cast<uint>(width) : 0;
}

inline uint CalculateTextboxHeight(const TextBoxStyle& style, uint text_height) {
	const TextBoxStyle::PaddingStyle& padding = style.padding;
	if (padding._top.IsPercent() || padding._down.IsPercent()) { ExceptionDialog(style_parse_exception); }
	int border_width = static_cast<int>(style.border._width);
	int height = static_cast<int>(text_height) + padding._top.AsSigned() + padding._down.AsSigned() + border_width * 2;
	return height > 0 ? static_cast<uint>(height) : 0;
}


END_NAMESPACE(WndDesign)