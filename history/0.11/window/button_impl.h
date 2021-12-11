#pragma once

#include "button.h"
#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)

class _Button_Impl : public Button, public _Wnd_Impl {

	void Destroy() override;

};


END_NAMESPACE(WndDesign)