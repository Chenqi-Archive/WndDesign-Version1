#pragma once

#include "../WndDesign.h"

BEGIN_NAMESPACE(WndDesignHelper)

using std::unique_ptr;


template<class IWndType>
class WndType : public ObjectBase {
private:
	using StyleType = typename IWndType::StyleType;
	unique_ptr<IWndType, typename IWndType::Deleter> _wnd;
public:
	WndType(const ObjectBase& parent, const StyleType& style) :
		_wnd(IWndType::Create(const_cast<Ref<IWndBase*>>(parent.GetWnd()), style, this)){
	}
	~WndType() {}
	Ref<const IWndBase*> GetWnd() const override { return _wnd.get(); }
	using ObjectBase::GetWnd;  // For the non-const version.
	Ref<const IWndType*> operator->() const { return _wnd.get(); }
	Ref<IWndType*> operator->() { return _wnd.get(); }
};


using WndBase = WndType<IWndBase>;
using Wnd = WndType<IWnd>;
using ScrollWnd = WndType<IScrollWnd>;
using TextBox = WndType<ITextBox>;

class Desktop : public ObjectBase {
public:
	Ref<const IWndBase*> GetWnd() const override { return &IDesktop::Get(); }
	Ref<const IDesktop*> operator->() const { return &IDesktop::Get(); }
	Ref<IDesktop*> operator->() { return &IDesktop::Get(); }
};
extern Desktop desktop;


// Used as fake parent window when creating an object.
const class NullWnd : public ObjectBase {} nullwnd;


END_NAMESPACE(WndDesignHelper)