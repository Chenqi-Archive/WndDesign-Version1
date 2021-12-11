#pragma once

#include "wnd_base.h"

#include "../style/textbox_style.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

using std::wstring;

// The static text box with a single format. Multiple paragraphs(lines) are permitted.
AbstractInterface ITextBox : virtual IWndBase{
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~ITextBox() {}

public:
	using StyleType = TextBoxStyle;

	WNDDESIGN_API static Alloc<ITextBox*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object = nullptr);

	static const uint text_length_max = 65535;

	// Set the text
	// The length of the text is expected to be less than text_length_max(65535), or it may be very inefficent.
	virtual void SetText(const wstring& text) pure;
	virtual const wstring& GetText() const pure;
};

END_NAMESPACE(WndDesign)
