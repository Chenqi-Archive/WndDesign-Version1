#pragma once

#include "wnd.h"

#include "../style/scroll_wnd_style.h"


BEGIN_NAMESPACE(WndDesign)

AbstractInterface IScrollWnd : virtual IWnd{
	using StyleType = ScrollWndStyle;
	WNDDESIGN_API static unique_ptr<IScrollWnd> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Add a child window with positioning style. 
	virtual bool AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) pure;
	bool AddChildFixed(Ref<IWndBase*> child_wnd) { return AddChild(child_wnd, Positioning::Fixed, padding_null); }
	bool AddChildStatic(Ref<IWndBase*> child_wnd) { return AddChild(child_wnd, Positioning::Static, padding_null); }
	bool AddChildSticky(Ref<IWndBase*> child_wnd, Padding padding) { return AddChild(child_wnd, Positioning::Sticky, padding); }
};


END_NAMESPACE(WndDesign)
