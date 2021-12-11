#pragma once

#include "window/window.h"
#include "style/style.h"
#include "texture/texture.h"

BEGIN_NAMESPACE(WndDesign)

class MessageBoxOkCancle {
private:
	Ref Wnd* frame;
	WndStyle frame_style;
	Ref TextBox* textbox;
	TextBoxStyle textbox_style;
	Ref Button* ok;
	ButtonStyle ok_style;
	Ref Button* cancle;
	ButtonStyle ok_style;

public:
	static bool Show(Wnd* parent, const wstring& msg) {
		// Block the message for parent.
	}
};


END_NAMESPACE(WndDesign)