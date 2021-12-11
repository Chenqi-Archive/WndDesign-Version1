#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)


// You can actually add child windows to Wnd.
AbstractInterface IWnd : virtual IWndBase{
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IWnd() {}

public:
	using StyleType = WndStyle;
	WNDDESIGN_API static Alloc<IWnd*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);
};


END_NAMESPACE(WndDesign)