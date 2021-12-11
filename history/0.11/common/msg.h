#pragma once

#include "core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)

#define MSG_BEGIN(type) MSG_BEGIN_##type

enum class Msg {
	MouseMove,
	LeftDown,
	LeftUp,
	RightDown,
	RightUp,

	Wheel,

	KeyDown,
	KeyUp,
};

using Handler = bool (*)(Msg msg, void* para);

struct MouseMsg {
	Point point;
	bool left_down : 1;
	bool right_down : 1;
	bool shift_down : 1;
	bool ctrl_down : 1;
	bool middle_down : 1;
};





END_NAMESPACE(WndDesign)
