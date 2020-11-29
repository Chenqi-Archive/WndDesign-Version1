#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


// Track mouse click messages.(LeftDown, no MouseMove, then LeftUp)
class MouseClickTracker {
private:
	bool _has_mouse_down;

public:
	MouseClickTracker() : _has_mouse_down(false) {}

	// Track message, if mouse clicks, return true.
	bool TrackMsg(Msg msg) {
		if (!IsMouseMsg(msg)) { return false; }
		switch (msg) {
		case Msg::LeftDown: _has_mouse_down = true; break;
		case Msg::MouseMove: if (_has_mouse_down == true) { _has_mouse_down = false; } break;
		case Msg::LeftUp:if (_has_mouse_down == true) { _has_mouse_down = false; return true; }break;
		}
		return false;
	}
};


END_NAMESPACE(WndDesignHelper)