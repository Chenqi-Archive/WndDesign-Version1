#pragma once

#include "wnd_base.h"

#include "../style/textbox_style.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

using std::wstring;

// The static text box with a single format. Multiple paragraphs(lines) are permitted.
struct TextBox : virtual WndBase {
	WNDDESIGN_API static Alloc TextBox* Create(Ref WndBase* parent,
											   const TextBoxStyle& style, 
											   const wstring& text = L"", 
											   Handler handler = nullptr);

	virtual void SetText(const wstring& text) pure;
	virtual const wstring& GetText() const pure;
};

END_NAMESPACE(WndDesign)
