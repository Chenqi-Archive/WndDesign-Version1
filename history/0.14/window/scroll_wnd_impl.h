#pragma once

#include "scroll_wnd.h"
#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)

class _Scroll_Wnd_Impl : virtual public ScrollWnd, virtual public _Wnd_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Scroll_Wnd_Impl(Alloc WndStyle* style, Handler handler, 
					 ScrollWndStyle::RegionStyle entire_region, Ref ScrollBar* scroll_bar);
	~_Scroll_Wnd_Impl() { SetScrollBar(nullptr); }


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
public:
	void SetCanvas(const Canvas& canvas) override;

	const Rect CalculateStickyLayerRegion(Rect relative_region, Padding padding);


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
private:
	Ref ScrollBar* _scroll_bar;

private:
	const Point GetPosition() const override { return _base_layer.GetCurrentPoint(); }
	void SetPosition(Point point) override;
	const Rect GetEntireRegion() const override { return _base_layer.GetAccessibleRegion(); }
	void SetScrollBar(Ref ScrollBar* scroll_bar) override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
public:
	bool AddChild(Ref WndBase* child_wnd) override { return AddChildFixed(child_wnd); }
	bool AddChild(Ref WndBase* child_wnd, Positioning positioning, Padding padding) override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	bool DispatchMessage(Msg msg, Para para) override;
};


END_NAMESPACE(WndDesign)