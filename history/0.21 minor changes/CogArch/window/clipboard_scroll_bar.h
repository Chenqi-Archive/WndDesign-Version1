#pragma once

#include "WndDesign.h"

	// A horizontal scroll bar implementation.
class SimpleScrollBarHorizontal : public Wnd {
private:
	ScrollWnd& _parent;

private:
	class Slider : public Button {
		SimpleScrollBarHorizontal& _frame;
		MouseDragTracker _mouse_drag_tracker;
	public:
		Slider(SimpleScrollBarHorizontal& frame);
		bool Handler(Msg msg, Para para) override;
	}_slider;

public:
	SimpleScrollBarHorizontal(ScrollWnd& parent);
	void ViewUpdated();  // Called by parent window when receives Msg::Scroll
};
