#pragma once

#include "../window/window.h"
#include "../common/feature.h"

BEGIN_NAMESPACE(WndDesign)


// A convenient wrapper class for Wnd objects.
// You can bind the wnd's handler with an object's handler to use internal resources.
class WndObject : Uncopyable {
private:
	static bool ObjectHandler(Ref WndBase* wnd, Msg msg, Para para) {
		Ref WndObject* object = reinterpret_cast<Ref WndObject*>(wnd->GetUserData());
		return object->Handler(wnd, msg, para);
	}
protected:
	// Bind the window's handler with the object's handler.
	void BindWindow(Ref WndBase* wnd) {
		wnd->SetUserData(this);
		wnd->SetHandler(ObjectHandler);
	}
protected:
	WndObject() {}
	~WndObject() {}
	virtual bool Handler(Ref WndBase* wnd, Msg msg, Para para) pure;
};


// An example to use WndObject.
class WndObjectDemo : public WndObject {
	Alloc Wnd* _wnd;
public:
	~WndObjectDemo() { SafeDestroy(_wnd); }
	void Init() {
		WndStyle style;
		style.size.normal(800px, 480px);
		style.position.SetHorizontalCenter().SetVerticalCenter();
		_wnd = Wnd::Create(nullptr, style, nullptr);
		BindWindow(_wnd);  // Bind the handler with the object's handler.
	}
	bool Handler(Ref WndBase* wnd, Msg msg, Para para) override {
		return true;
	}
};


END_NAMESPACE(WndDesign)