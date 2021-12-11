#include "SimpleScrollWnd.h"


BEGIN_NAMESPACE(WndDesignHelper)


constexpr int WHEEL_DELTA = 120;  // Win32 default wheel delta.


SimpleScrollWnd::SimpleScrollWnd(const ObjectBase& parent, const ScrollWndStyle& style) :
	ScrollWnd(parent, style),
	_scroll_animation(*this),
	_scroll_bar(*this) {
	_scroll_bar.ViewUpdated();
}

bool SimpleScrollWnd::Handler(Msg msg, Para para) {
	if (msg == Msg::MouseWheel) {
		const MouseMsg& msg = GetMouseMsg(para);
		_scroll_animation.AddUp(-msg.wheel_delta * _scroll_delta_for_mouse_wheel / WHEEL_DELTA); 
		// Wheel_delta is positive when scroll away, so the view_delta is negative.
		return true;
	}
	if (msg == Msg::KeyDown) { // For up and down keys.
		KeyMsg key_msg = GetKeyMsg(para);
		if (key_msg.key == Key::Down) {
			_scroll_animation.AddUp(_scroll_delta_for_key);
		} else if (key_msg.key == Key::Up) {
			_scroll_animation.AddUp(-_scroll_delta_for_key);
		}
		return true;
	}
	
	int wheel_delta = _scroll_animation.TrackTimerMsg(msg, para);
	if (wheel_delta != 0) {
		(*this)->ScrollView(Vector(0, wheel_delta));
		return true;
	}

	if (msg == Msg::Scroll) {
		_scroll_bar.ViewUpdated();
		return true;
	}
	return false;
}


END_NAMESPACE(WndDesignHelper)
