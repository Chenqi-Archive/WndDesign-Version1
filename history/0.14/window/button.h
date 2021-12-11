#pragma once

#include "textbox.h"

#include "../style/button_style.h"

BEGIN_NAMESPACE(WndDesign)

// You will only receive Msg::LeftUp when you clicked down and up.

struct Button : virtual TextBox{
	WNDDESIGN_API static Alloc Button* Create(Ref WndBase* parent, 
											  const ButtonStyle& style, 
											  Handler handler,
											  const wstring& text = L"");
};

END_NAMESPACE(WndDesign)
