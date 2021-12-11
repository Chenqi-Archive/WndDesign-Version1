#pragma once

#include "Button.h"
#include "../widget/mouse_drag_tracker.h"

BEGIN_NAMESPACE(WndDesignHelper)


// A vertical scroll bar implementation.
class SimpleScrollBar : public Wnd {
private:
	ScrollWnd& _parent;

private:
	class Slider : public Button {
		SimpleScrollBar& _frame;
		MouseDragTracker _mouse_drag_tracker;
	public:
		Slider(SimpleScrollBar& frame);
		bool Handler(Msg msg, Para para) override;
	}_slider;

public:
	SimpleScrollBar(ScrollWnd& parent);
	void ViewUpdated();  // Called by parent window when receives Msg::Scroll
};


END_NAMESPACE(WndDesignHelper)