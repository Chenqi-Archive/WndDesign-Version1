#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)

enum class Msg : uint {
	// Message				// Parameters:			// Descriptions(Optional)
	_MSG_BEGIN,


	//// Window Message ////
	_WINDOW_MSG_BEGIN,

	//Create,				// nullptr
	//Destroy,				// nullptr

	//Attach,				// Ref<WndBase*> (parent window) (Sent when the window is added as a child.)
	//Detach,				// Ref<WndBase*> (previous parent window) (Sent when the window was removed from parent window.)

	//Resize,				// Ref<Size*> (Sent when the window's size has changed.)

	Scroll,					// nullptr (For ScrollWnd only. (And also Table)
							//          The message is sent when entire scrollable region has changed or the window 
							//            resizes(along with Msg::Resize), usually used to redraw scroll bar.
							//          You can call interface functions of ScrollWnd to get the new regions. 
	
	_WINDOW_MSG_END,


	//// Mouse Message ////
	_MOUSE_MSG_BEGIN,

	LeftDown,				// Ref<MouseMsg*>
	LeftUp,					// Ref<MouseMsg*>
	RightDown,				// Ref<MouseMsg*>
	RightUp,				// Ref<MouseMsg*>
	MouseMove,				// Ref<MouseMsg*>
	MouseWheel,				// Ref<MouseMsg*>
	MouseWheelHorizontal,	// Ref<MouseMsg*>

	ChildHit,				// Ref<MouseMsgChildHit*>  (As a notification to parent window before sent to child window.  
							//							Return false, and the message will not be sent to child).

	_MOUSE_MSG_END,


	//// Supplement Mouse Message ////
	_SUPPLEMENT_MOUSE_MESSAGE_BEGIN,

	HitTest,				// Ref<Point*>  (Msg::HitTest will be sent before a real mouse message is sent, 
							//                 if you returns false, the real message will not be sent)
	MouseLeave,				// nullptr
	LoseCapture,			// nullptr

	_SUPPLEMENT_MOUSE_MESSAGE_END,


	//// Keyboard Message ////
	_KEYBOARD_MSG_BEGIN,

	KeyDown,				// Ref<KeyMsg*>
	KeyUp,					// Ref<KeyMsg*>

	Char,				    // wchar (utf-16 character code. You will only get visible character key messages by GetCharMsg(). )
							// Visible character key messages are expected to be handled in Msg::Char,
							//   while control key messages be handled in Msg::KeyDown.

	ImeStartComposition,	// nullptr					  (Remember the caret position when starts composition)
	ImeComposition,			// Ref<const ImeComposition*> (Replace the composition string every time composition updates) 
	ImeEndComposition,		// Ref<const ImeComposition*> (Commit the result string and reset caret position)
	
	_KEYBOARD_MSG_END,


	// Supplement keyboard Message
	_SUPPLEMENT_KEYBOARD_MESSAGE_BEGIN,

	LoseFocus,				// nullptr

	_SUPPLEMENT_KEYBOARD_MSG_END,


	//// Timer Message ////
	Timer,					// Ref<Timer*>


	//// User Message ////
	_USER_MSG_BEGIN = 0x80000000,


	_MSG_END = 0xFFFFFFFF
};


constexpr inline Msg UserMsg(uint number) {
	return static_cast<Msg>(static_cast<uint>(Msg::_USER_MSG_BEGIN) + number);
}


struct IWndBase;  // IWndBase declaration.
using Para = Ref<void*>;


END_NAMESPACE(WndDesign)
