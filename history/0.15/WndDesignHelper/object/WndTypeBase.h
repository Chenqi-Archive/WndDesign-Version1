#pragma once

#include "../WndDesign.h"

BEGIN_NAMESPACE(WndDesignHelper)


template<class IWndType>
class WndType : public ObjectBase {
	using StyleType = typename IWndType::StyleType;
private:
	Alloc<IWndType*> _wnd;
public:
	WndType() :_wnd(nullptr) {}
	~WndType() { Destroy(); }
	void Destroy() { if (_wnd) { _wnd->Destroy(); _wnd = nullptr; } }
	void Init(Ref<ObjectBase*> parent, StyleType style) {
		Destroy();  // Clean allocated resources.
		auto parent_wnd = parent == nullptr ? nullptr : parent->GetWnd();
		_wnd = IWndType::Create(parent_wnd, style, this);
	}
	bool IsEmpty() { return _wnd == nullptr; }
	Ref<IWndBase*> GetWnd() override { return _wnd; }
	Ref<IWndType*> operator->() { return _wnd; }
};


using WndBase = WndType<IWndBase>;
using Wnd = WndType<IWnd>;
using ScrollWnd = WndType<IScrollWnd>;
using TextBox = WndType<ITextBox>;

class Desktop : public WndType<IDesktop> {
private:
	bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) { return false; }
public:
	void Init() { WndType<IDesktop>::Init(nullptr, 0); }
};


extern Desktop desktop;


END_NAMESPACE(WndDesignHelper)