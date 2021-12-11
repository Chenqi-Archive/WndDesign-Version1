#pragma once

#include "wnd.h"

BEGIN_NAMESPACE(WndDesign)


struct Desktop : virtual Wnd {
	// Add a new dialog window and block the message input to parent window until the dialog is closed.
	virtual void AddModalDialogBox(const Ref WndBase* dialog, const Ref WndBase* parent) pure;


};

extern Desktop* desktop;

END_NAMESPACE(WndDesign)
