#pragma once

#include "font_style.h"
#include "wnd_style.h"

BEGIN_NAMESPACE(WndDesign)


// For static text boxes with a single font format.
struct TextBoxStyle : WndStyle{
	// The padding between content and the border.
	struct PaddingStyle {
		uchar left = 0px;
		uchar right = 0px;
		uchar top = 0px;
		uchar down = 0px;
		void Set(uchar all) { left = right = top = down = all; }
	}padding;

	// Font
	FontStyle font;
	uchar letter_space = 0px;
	uchar word_space = 0px;

	// Paragraph
	uchar line_height = 0px;  // 100 percent
	uchar indent = 0px;          // The indent for each paragraph.
	uchar paragraph_space = 0px;
	
	enum class TextAlign : uchar {
		Left,
		Right,
		Center,
		Justify
	}align = TextAlign::Center;
	
	enum class TextDirection : uchar {
		LeftToRight,
		RightToLeft,
	}direction = TextDirection::LeftToRight;
	
	enum class WhiteSpace : uchar {
		Normal,
		Pre,
		NoWrap,
		PreWrap,
		PreLine
	}white_space = WhiteSpace::Normal;
	
	enum class TextOverflow : uchar {
		Clip,
		Ellipsis,
	}overflow = TextOverflow::Clip;
};


//
//constexpr uint infinite_length = (uint)-1;
//
//struct _TextRange {
//	FontStyle font = FontStyle();
//	uint begin = 0;
//	uint length = infinite_length;
//};


END_NAMESPACE(WndDesign)
