#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


// Tracks ctrl+key message.
class CtrlTracker {
	bool _has_ctrl_down = false;
public:
	Key TrackMsg(Msg msg, Para para) {
		if (msg == Msg::KeyDown && GetKeyMsg(para).key == Key::Ctrl) {
			_has_ctrl_down = true;
		} else if (msg == Msg::KeyUp && GetKeyMsg(para).key == Key::Ctrl) {
			_has_ctrl_down = false;
		} else if (msg == Msg::KeyDown && _has_ctrl_down) {
			return GetKeyMsg(para).key;
		}
		return Key::Undefined;
	}
};


END_NAMESPACE(WndDesignHelper)