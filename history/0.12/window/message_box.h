#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

// A wrapper control with a textbox and at most three buttons.

struct MessageBoxStyle : WndStyle{
	TextBoxStyle text_box_style;
	ButtonStyle button_style;
};


struct MessageBox : WndBase {
	WNDDESIGN_API static Alloc MessageBox* Create(const MessageBoxStyle& style, Handler handler);

	// The message handler only receives LeftUp messages with the parameter as the button id (0, 1, 2).
	static inline int GetButtonID(Para para) { return reinterpret_cast<int>(para); }

	virtual void SetText(const wstring& text) pure;
	virtual void AddButton(const wstring& text) pure;
};


END_NAMESPACE(WndDesign)