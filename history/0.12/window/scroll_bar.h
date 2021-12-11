#pragma once

#include "button.h"

BEGIN_NAMESPACE(WndDesign)

class ScrollWnd;

class ScrollBar {
private:
	// The parent scroll window.
	const Ref ScrollWnd* _parent;
	Rect accessible_region;
	Rect current_region;

public:
	ScrollBar();
	virtual ~ScrollBar();
	virtual bool Init(Ref ScrollWnd* parent, Rect accessible_region, Rect current_region) pure;
	virtual void SetAccessibleRegion(Rect accessible_region) pure;
	virtual void SetCurrentRegion(Rect current_region) pure;
};



// A wrapper class with many wnd objects.

class ScrollBarImpl : public ScrollBar{
private:
	// The vertical scroll bar.
	Alloc Wnd* _vertical;
	Alloc Button* _vertical_button;

	//// The horizontal scroll bar.
	//Alloc Wnd* _horizontal;
	//Alloc Button* _horizontal_button;

	//// The corner window.
	//Alloc Wnd* _corner;

private:
	static bool vertical_bar_handler(WndBase* wnd, Msg msg, Para para) {

	}
	static bool vertical_button_handler(WndBase* wnd, Msg msg, Para para) {

	}

public:
	~ScrollBarImpl() override;
	bool Init(Ref ScrollWnd* parent, Rect accessible_region, Rect current_region) override {
		WndStyle vertical_style;
		vertical_style.size.normal = { 10px, 100pct };
		vertical_style.position.right = 0px;
		vertical_style.position.SetVerticalCenter();
		vertical_style.background = &built_in_resources.color_white;
		_vertical = Wnd::Create(parent, vertical_style, vertical_bar_handler);

		ButtonStyle vertical_button_style;
		vertical_button_style
		vertical_button_style
		vertical_button_style
		vertical_button_style
	}
	void SetAccessibleRegion(Rect accessible_region) override;
	void SetCurrentRegion(Rect current_region) override;
};


END_NAMESPACE(WndDesign)