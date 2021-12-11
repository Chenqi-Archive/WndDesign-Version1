#pragma once

#include "text_box.h"

#include "../style/button_style.h"

BEGIN_NAMESPACE(WndDesign)

// Receives mouse message: LeftUp only when really clicked on(clicked down and then up). 

struct Button : virtual TextBox{
	WNDDESIGN_API static Alloc Button* Create(Ref Wnd* parent, const ButtonStyle& style, Handler handler, const wstring& text = L"");
};

END_NAMESPACE(WndDesign)
