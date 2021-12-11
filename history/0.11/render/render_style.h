#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"

#include "../style/style.h"

BEGIN_NAMESPACE(WndDesign)

struct RenderStyle {
	enum {Wnd, Text} type;

	Point left_top;
	Size size;

	Texture* background;

	// Effect

};

struct LayerStyle {
	uchar opacity = 0xFF;


};

struct RenderStyleText {

};


END_NAMESPACE(WndDesign)