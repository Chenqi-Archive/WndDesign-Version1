#include "../WndDesign/WndDesign.h"
#include "../WndDesign/Helper.h"

using namespace WndDesign;


class WndObject{
private:
	static bool _handler(WndBase* wnd, Msg msg, Para para){  // WndObject* 
		WndObject* wnd_object = reinterpret_cast<WndObject*>(wnd->GetUserData());
		wnd_object->Handler(msg, para);
	}
protected:
	WndBase* wnd;
public:
	WndObject(Wnd* parent = nullptr) {
		WndStyle style;
		StyleInit(style);
		wnd = WndBase::Create(parent, style, _handler);
		wnd->SetUserData(reinterpret_cast<void*>(this));
	}
	~WndObject() { SafeDestroy(&wnd); }
	virtual void StyleInit(WndStyle& style) pure;
	virtual bool Handler(Msg msg, Para para) pure;
};


class MainFrame : public WndObject{
	void StyleInit(WndStyle& style) override {
		style.size.SetFixed(1000px, 800px);
	}
	bool Handler(Msg msg, Para para) override {
		if (msg == Msg::LeftDown) {

		}
		return true;
	}
}main_frame;