#pragma once

#include "wnd_style.h"

BEGIN_NAMESPACE(WndDesign)


class ScrollBar;

struct ScrollWndStyle : WndStyle {
	Rect scroll_region = region_infinite;
	const Ref ScrollBar* scroll_bar = &built_in_resources.default_scroll_bar;
};


END_NAMESPACE(WndDesign)
