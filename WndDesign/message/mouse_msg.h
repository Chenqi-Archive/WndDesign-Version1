#pragma once

#include "msg.h"

BEGIN_NAMESPACE(WndDesign)


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
		unsigned short _key_state;
	};
	short wheel_delta;
};


struct MouseMsgChildHit : MouseMsg {
	Msg original_msg;
	IWndBase* child;
};


inline bool IsMouseMsg(Msg msg) {
	return msg > Msg::_MOUSE_MSG_BEGIN && msg < Msg::_MOUSE_MSG_END;
}

inline const MouseMsg& GetMouseMsg(Para para) {
	return *reinterpret_cast<Ref<MouseMsg*>>(para);
}

inline const MouseMsgChildHit& GetMouseMsgChildHit(Para para) {
	return *reinterpret_cast<Ref<MouseMsgChildHit*>>(para);
}

inline void ConvertChildHitMsgToOriginalMsg(Msg& msg, Para para) {
	if (msg == Msg::ChildHit) {
		msg = GetMouseMsgChildHit(para).original_msg;
	}
}

END_NAMESPACE(WndDesign)