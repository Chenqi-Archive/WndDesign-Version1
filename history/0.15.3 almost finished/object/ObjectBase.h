#pragma once

#include "../window/wnd_base.h"


BEGIN_NAMESPACE(WndDesignHelper)


using namespace WndDesign;


// A convenient wrapper class for window objects.
AbstractInterface ObjectBase : Uncopyable {
	virtual bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) pure;
	virtual Ref<IWndBase*> GetWnd() { return nullptr; }
};


END_NAMESPACE(WndDesignHelper)