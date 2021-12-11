#pragma once

#include "WndTypeBase.h"
#include "SimpleScrollBar.h"

#include "../widget/moving_animation.h"


BEGIN_NAMESPACE(WndDesignHelper)


class SimpleScrollWnd : public ScrollWnd {
private:
	// For scrolling animation.
	MovingAnimation _scroll_animation;
	static const uint _scroll_time = 100;  // ms
	static const int _scroll_delta_for_mouse_wheel = 120; // px
	static const int _scroll_delta_for_key = 60;  // px

	// Scroll bar.
	SimpleScrollBar _scroll_bar;
public:
	SimpleScrollWnd(const ObjectBase& parent, const ScrollWndStyle& style);
	bool Handler(Msg msg, Para para) override;
};


END_NAMESPACE(WndDesignHelper)
