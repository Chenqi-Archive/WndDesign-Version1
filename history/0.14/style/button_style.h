#pragma once

#include "../geometry/geometry.h"
#include "textbox_style.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

struct ButtonStyle : TextBoxStyle {
	Color32 background_hover;
	Color32 background_down;

	ButtonStyle() {
		padding.SetAll(5px);
		background = ColorSet::LightGray;
		background_hover = ColorSet::Gray;
		background_down = ColorSet::DimGray;
	}
};


END_NAMESPACE(WndDesign)
