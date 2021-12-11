#pragma once

#include "core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)

enum class Msg : uint{
	LoseFocus,  // The fucus is set when leftdown, when leftdown on other windows, the focus loses.

	_MOUSE_MSG_BEGIN,
	MouseMove,
	MouseWheel,  // Sent to the window on focus.
	LeftDown,
	LeftUp,
	RightDown,
	RightUp,
	_MOUSE_MSG_END,

	KeyDown,
	KeyUp,
};

using Para = Ref void*;
struct WndBase;
using Handler = bool (*)(WndBase* wnd, Msg msg, Para para);


struct MouseMsg {  // 12 bytes
	Point point;
	union {
		struct {
			bool left : 1;
			bool right : 1;
			bool shift : 1;
			bool ctrl : 1;
			bool middle : 1;
			bool xbutton1 : 1;
			bool xbutton2 : 1;
		};
		unsigned short virtual_keys;
	};
	short wheel_delta;
};

inline bool IsMouseMsg(Msg msg) {
	return msg > Msg::_MOUSE_MSG_BEGIN && msg < Msg::_MOUSE_MSG_END;
}

inline const MouseMsg& GetMouseMsg(Para para) {
	return *reinterpret_cast<MouseMsg*>(para);
}

END_NAMESPACE(WndDesign)
