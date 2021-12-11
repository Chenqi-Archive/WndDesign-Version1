#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)


// You can actually add child windows to Wnd.
AbstractInterface IWnd : virtual IWndBase{
	using StyleType = WndStyle;
	WNDDESIGN_API static unique_ptr<IWnd> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);
};


END_NAMESPACE(WndDesign)