#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)

enum class Msg : uint {
	// Messages:			// Parameters:
	_MSG_BEGIN,

	// Window Message
	Create,					// nullptr
	Destroy,				// nullptr
	Scroll,					// nullptr (For ScrollWnd only)

	// Mouse Message
	_MOUSE_MSG_BEGIN,          
	LeftDown,				// ref<MouseMsg*>
	LeftUp,					// ref<MouseMsg*>
	RightDown,				// ref<MouseMsg*>
	RightUp,				// ref<MouseMsg*>
	MouseMove,				// ref<MouseMsg*>
	MouseWheel,				// ref<MouseMsg*>
	_MOUSE_MSG_END,

	// Supplement Mouse Message
	HitTest,				// ref<Point*>  (Msg::HitTest will be sent before a real mouse message is sent, 
							//            if the handler returns false, the real message will not be sent)
	MouseLeave,				// nullptr
	LoseCapture,			// nullptr

	// Keyboard Message
	KeyDown,
	KeyUp,
	Char,				    // (utf-32 code)
	LoseFocus,				// nullptr

	// Timer Message
	Timer,					// ref<Timer*>

	// User Message
	_USER_MSG_BEGIN = 0x80000000,

	_MSG_END
};


constexpr inline Msg _UserMsg(uint number) {
	return static_cast<Msg>(static_cast<uint>(Msg::_USER_MSG_BEGIN) + number);
}
#define UserMsg(number) _UserMsg(number)


using Para = Ref<void*>;


END_NAMESPACE(WndDesign)
