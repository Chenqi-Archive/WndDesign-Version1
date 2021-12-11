#pragma once

#include "wnd_base.h"
#include "text_box.h"

BEGIN_NAMESPACE(WndDesign)

struct EditBoxStyle :TextBoxStyle {

};

// The editable and scrollable text box, a wrapper control with one textbox for each paragraph.

struct EditBox : virtual WndBase{
	WNDDESIGN_API static Alloc EditBox* Create(Ref Wnd* parent, const Ref EditBoxStyle* style, Handler handler);

	virtual void SetExtendedStyle(const Ref EditBoxStyle* style) pure;

};


END_NAMESPACE(WndDesign)
