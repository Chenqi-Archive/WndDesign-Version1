#include "SimpleScrollWnd.h"


BEGIN_NAMESPACE(WndDesignHelper)

bool SimpleScrollWnd::Handler(Ref<IWndBase*> wnd, Msg msg, Para para) {
	if (msg == Msg::MouseWheel) {
		_scroll_timer.Set(scroll_animation_period);

		const MouseMsg& msg = GetMouseMsg(para);
		_wheel_left += -msg.wheel_delta;  // Wheel_delta is positive when scroll away, so the view_delta is negative.
		_wheel_delta_default = _wheel_left * scroll_animation_period / 100;
		return true;
	}
	if (msg == Msg::Timer && GetTimerMsg(para) == &_scroll_timer) {
		int wheel_delta;
		if (_wheel_left > 0) {
			wheel_delta = min(_wheel_left, _wheel_delta_default);
		} else {
			wheel_delta = max(_wheel_left, _wheel_delta_default);
		}
		_wheel_left -= wheel_delta;
		if (_wheel_left == 0) { _scroll_timer.Stop(); }
		(*this)->MoveView({ 0, wheel_delta });
		return true;
	}
	if (msg == Msg::Scroll) {
		_scroll_bar.ViewUpdated();
		return true;
	}
	return false;
}


END_NAMESPACE(WndDesignHelper)
