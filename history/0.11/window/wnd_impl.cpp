#include "wnd_impl.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

Wnd* Wnd::Create(Wnd* parent, const WndStyle* style, Handler handler) {
	_Wnd_Impl* p = dynamic_cast<_Wnd_Impl*>(parent);
	if (p == nullptr) {
		return nullptr;
	}
	_Wnd_Impl* wnd = new _Wnd_Impl(p, style, handler);
	p->AddChild(wnd);
	return wnd;
}

END_NAMESPACE(WndDesign)