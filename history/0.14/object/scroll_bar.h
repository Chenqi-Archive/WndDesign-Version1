#pragma once

#include "wnd_object.h"

BEGIN_NAMESPACE(WndDesign)


struct ScrollWnd;

class ScrollBar : public WndObject{
protected:
	Ref ScrollWnd* _parent; // The parent scroll window.
	Rect _entire_region;
	Rect _visible_region;
	void RefreshRegion();
public:
	ScrollBar();
	~ScrollBar() { Clear(); }
	virtual void Init(Ref ScrollWnd* parent) pure;
	virtual void Clear() pure;
	virtual void ViewUpdated() pure; // Only called by parent window when its position is reset.
};


// A vertical scroll bar implementation.
class SimpleScrollBar : public ScrollBar {
private:
	Alloc Wnd* _frame;
	Alloc Button* _slider;
	bool _has_mouse_down;
	Point _mouse_down_position;

public:
	SimpleScrollBar();
	void Init(Ref ScrollWnd* parent) override;
	void Clear() override;
	void ViewUpdated() override;
	bool Handler(Ref WndBase* wnd, Msg msg, Para para) override;
};


END_NAMESPACE(WndDesign)