#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"
#include "textbox_style.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

struct ButtonStyle : TextBoxStyle {
	Texture* background_hover;
	Texture* background_down;

	ButtonStyle() {
		padding.Set(5px);
		indent = 0px;
		white_space = WhiteSpace::NoWrap;
		background = &built_in_resources.color_light_gray;
		background_hover = &built_in_resources.color_gray;
		background_down = &built_in_resources.color_dim_gray;
	}
};


END_NAMESPACE(WndDesign)
