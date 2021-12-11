#pragma once

#include "../geometry/geometry.h"
#include "font_style.h"

#include <string>
#include <vector>

BEGIN_NAMESPACE(WndDesign)


struct TextBoxStyle {
	struct TextRange {
		FontStyle style;
		uint begin, end;
	};

	std::wstring text = L"Click";
	std::vector<TextRange> styles;


	Size size = { 50px, 25px };
	Point left_top = { 100px, 100px };

	//uchar border_weight = 0px;

	uchar padding_top = 10px;
	uchar padding_down = 10px;
	uchar padding_left = 20px;
	uchar padding_right = 20px;
};


END_NAMESPACE(WndDesign)
