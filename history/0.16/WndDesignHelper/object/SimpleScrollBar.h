#pragma once

#include "Button.h"

BEGIN_NAMESPACE(WndDesignHelper)


// A vertical scroll bar implementation.
class SimpleScrollBar : public Wnd {
private:
	ScrollWnd& _parent;

private:
	class Slider : public Button {
		SimpleScrollBar& _frame;
		bool _has_mouse_down;
		Point _mouse_down_position;
	public:
		Slider(SimpleScrollBar& frame) :_frame(frame), 
			_has_mouse_down(false), _mouse_down_position(point_zero) {}
		void Destroy() {
			_has_mouse_down = false;
			_mouse_down_position = point_zero;
			Button::Destroy();
		}
		bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) override;
	}_slider;

public:
	SimpleScrollBar(ScrollWnd& parent) : _parent(parent), _slider(*this) {}
	~SimpleScrollBar() { Destroy(); }
	void Init();
	void Destroy();
	void ViewUpdated();  // Called by parent window when receives Msg::Scroll
	bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) override;
};


END_NAMESPACE(WndDesignHelper)