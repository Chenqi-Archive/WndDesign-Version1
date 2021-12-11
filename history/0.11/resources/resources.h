#pragma once

#include "../texture/color.h"



BEGIN_NAMESPACE(WndDesign)


struct BuiltInResources {
	SolidColor color_light_gray = Color::LightGray;
	SolidColor color_gray = Color::Gray;
	SolidColor color_dim_gray = Color::DimGray;
	SolidColor color_white = Color::White;
	SolidColor color_black = Color::Black;
	// Default styles.


};


extern BuiltInResources built_in_resources;


END_NAMESPACE(WndDesign)
