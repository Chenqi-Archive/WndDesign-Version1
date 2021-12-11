#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

struct Wnd : virtual WndBase {
	WNDDESIGN_API static Alloc Wnd* Create(Ref Wnd* parent, const WndStyle& style, Handler handler);


	// Add the child window.
	virtual bool AddChild(Ref WndBase* child_wnd) pure;
	// Remove the child window, but not destroy the window.
	virtual bool RemoveChild(Ref WndBase* child_wnd) pure;
};

END_NAMESPACE(WndDesign)
