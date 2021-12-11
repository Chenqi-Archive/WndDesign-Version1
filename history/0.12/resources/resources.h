#pragma once

#include "../texture/color.h"
#include "../window/scroll_bar.h"


BEGIN_NAMESPACE(WndDesign)


struct BuiltInResources {
	// Common colors.
	SolidColor color_light_gray;
	SolidColor color_gray;
	SolidColor color_dim_gray;
	SolidColor color_white;
	SolidColor color_black;
	
	WndStyle default_wnd_style;

	ScrollBar default_scroll_bar;

	BuiltInResources() {
		color_light_gray = ColorSet::LightGray;
		color_gray = ColorSet::Gray;
		color_dim_gray = ColorSet::DimGray;
		color_white = ColorSet::White;
		color_black = ColorSet::Black;
	}
};


extern BuiltInResources built_in_resources;


END_NAMESPACE(WndDesign)
