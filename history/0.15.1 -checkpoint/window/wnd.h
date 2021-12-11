#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

AbstractInterface IWnd : virtual IWndBase{
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IWnd() {}

public:
	using StyleType = WndStyle;

	WNDDESIGN_API static Alloc<IWnd*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);
	

	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Reset the child window's region after the position or size has changed.
	// Returns true if the window's actual region has really changed.
	virtual bool MoveChild(Ref<IWndBase*> child_wnd) pure;
	// Reset the child window's render style after the render style has changed.
	virtual void SetChildRenderStyle(Ref<IWndBase*> child_wnd) pure;
};

END_NAMESPACE(WndDesign)
