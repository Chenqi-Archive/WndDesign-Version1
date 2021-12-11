#pragma once

#include "WndTypeBase.h"
#include "SimpleScrollBar.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


class SimpleScrollWnd : public ScrollWnd {
private:
	// For scrolling animation.
	static const int scroll_animation_period = 10;  // ms
	Timer _scroll_timer;
	int _wheel_delta_default = 0;
	int _wheel_left = 0;

	// Scroll bar.
	SimpleScrollBar _scroll_bar;

public:
	SimpleScrollWnd() :_scroll_timer(*this), _scroll_bar(*this) {}
	void Init(Ref<ObjectBase*> parent, ScrollWndStyle style) {
		ScrollWnd::Init(parent, style);
		_scroll_bar.Init();
	}
	void Destroy() {
		_scroll_bar.Destroy();
		ScrollWnd::Destroy();
	}
	bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) override;
};


END_NAMESPACE(WndDesignHelper)
