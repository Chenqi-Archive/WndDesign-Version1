#pragma once

#include "window_base.h"


class MainFrame : private ScrollWndObject{
private:


public:
	void Init() {
		WndStyle style;
		style.size.normal(800px, 480px);
		style.position.SetHorizontalCenter().SetVerticalCenter();
		ScrollWndObject::Init(desktop, style);
	}
	void Destroy() { ScrollWndObject::Destroy(); }
	bool Handler(IWndBase* wnd, Msg msg, Para para) override {
		if (ScrollWndObject::Handler(wnd, msg, para)) {
			return true;
		}

		return true;
	}

};