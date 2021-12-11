#pragma once

#include "wnd_object.h"

BEGIN_NAMESPACE(WndDesign)

// A wrapper control with a textbox and at most three buttons.
//
//class MessageBoxOkCancle {
//private:
//	Ref<Wnd* frame;
//	WndStyle frame_style;
//	Ref<TextBox* textbox;
//	TextBoxStyle textbox_style;
//	Ref<Button* ok;
//	ButtonStyle ok_style;
//	Ref<Button* cancle;
//	ButtonStyle cancle_style;
//
//public:
//	static bool Show(Wnd* parent, const wstring& msg) {
//		// Block the message for parent.
//	}
//};

struct MessageBoxStyle : WndStyle{
	TextBoxStyle text_box_style;
	ButtonStyle button_style;
};


struct MessageBox : IWndBase {
	WNDDESIGN_API static Alloc<MessageBox* Create(const MessageBoxStyle& style, Handler handler);

	// The message handler only receives LeftUp messages with the parameter as the button id (0, 1, 2).
	static inline int GetButtonID(Para para) { return static_cast<int>(reinterpret_cast<long long>(para)); }

	virtual void SetText(const wstring& text) pure;
	virtual void AddButton(const wstring& text) pure;
};


END_NAMESPACE(WndDesign)