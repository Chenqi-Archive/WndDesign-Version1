#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)

enum class Msg : uint {
	// Message				// Parameters:			// Descriptions(Optional)
	_MSG_BEGIN = 0,

	// Window Message
	_WINDOW_MSG_BEGIN,
	Create,					// nullptr
	Destroy,				// nullptr
	Resize,					// ref<Size*> (Sent when the window's size has changed.)
	Scroll,					// nullptr (For ScrollWnd only. 
							//          The message is sent when entire scrollable region has changed or the window 
							//            resizes(along with Msg::Resize), usually used to redraw scroll bar.
							//          You can call interface functions of ScrollWnd to get the new regions. 


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
							//                 if you returns false, the real message will not be sent)
	MouseLeave,				// nullptr
	LoseCapture,			// nullptr


	// Keyboard Message
	KeyDown,				// ref<KeyMsg*>
	KeyUp,					// ref<KeyMsg*>
	Char,				    // wchar (utf-16 character code, also including control charaters that you should filter out.
	LoseFocus,				// nullptr


	// IME Message			(For editbox)
	ImeStartComposition,	// nullptr					  (Remember the caret position when starts composition)
	ImeComposition,			// ref<const ImeComposition*> (Replace the composition string every time composition updates) 
	ImeEndComposition,		// ref<const ImeComposition*> (Commit the result string and reset caret position)


	// Timer Message
	Timer,					// ref<Timer*>

	// User Message
	_USER_MSG_BEGIN = 0x80000000,


	_MSG_END = 0xFFFFFFFF
};


constexpr inline Msg _UserMsg(uint number) {
	return static_cast<Msg>(static_cast<uint>(Msg::_USER_MSG_BEGIN) + number);
}
#define UserMsg(number) _UserMsg(number)


using Para = Ref<void*>;


END_NAMESPACE(WndDesign)
