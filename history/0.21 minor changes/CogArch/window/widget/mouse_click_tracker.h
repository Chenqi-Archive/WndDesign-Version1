#pragma once

#include "../WndDesign.h"
#include "../helper.h"


// Track mouse click messages.(LeftDown, no MouseMove, then LeftUp)
class MouseClickTrackerWithTolerance {
private:
	bool _has_mouse_down;
	Point _mouse_down_position;

public:
	MouseClickTrackerWithTolerance() : _has_mouse_down(false), _mouse_down_position(point_zero) {}

	// Track message, if mouse clicks, return true.
	bool TrackMsg(Msg msg, Para para) {
		if (!IsMouseMsg(msg)) { return false; }
		auto& mouse_msg = GetMouseMsg(para);
		switch (msg) {
		case Msg::LeftDown: _has_mouse_down = true; _mouse_down_position = mouse_msg.point; break;
		case Msg::MouseMove:
			if (_has_mouse_down == true && SquareLength(mouse_msg.point - _mouse_down_position) >= 10 * 10) {
				_has_mouse_down = false;
			} break;
		case Msg::LeftUp:if (_has_mouse_down == true) { _has_mouse_down = false; return true; }break;
		}
		return false;
	}
};
