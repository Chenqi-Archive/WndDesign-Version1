#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"
#include "textbox_style.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

struct ButtonStyle : TextBoxStyle{
	Texture* normal = &built_in_resources.color_light_gray;
	Texture* hover = &built_in_resources.color_gray;
	Texture* down = &built_in_resources.color_dim_gray;
};


END_NAMESPACE(WndDesign)
