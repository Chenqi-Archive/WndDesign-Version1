#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

struct Wnd : virtual WndBase {
	WNDDESIGN_API static Alloc Wnd* Create(Ref WndBase* parent, const WndStyle& style, Handler handler);
	

	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Reset the child window's region after the position or size has changed.
	virtual void MoveChild(Ref WndBase* child_wnd) pure;
	// Reset the child window's render style after the render style has changed.
	virtual void SetChildRenderStyle(Ref WndBase* child_wnd) pure;
};

END_NAMESPACE(WndDesign)
