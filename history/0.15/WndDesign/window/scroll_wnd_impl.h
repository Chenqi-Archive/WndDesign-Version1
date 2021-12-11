#pragma once

#include "scroll_wnd.h"
#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)

class _Scroll_Wnd_Impl : virtual public IScrollWnd, virtual public _Wnd_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Scroll_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object, ScrollWndStyle::RegionStyle entire_region);
	~_Scroll_Wnd_Impl() {}


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
	const Point GetPosition() const override { return _base_layer.GetCurrentPoint(); }
	void SetPosition(Point point) override;
	const Rect GetEntireRegion() const override { return _base_layer.GetAccessibleRegion(); }


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
public:
	bool AddChild(Ref<IWndBase*> child_wnd) override { return AddChildFixed(child_wnd); }
	bool AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) override;
};


END_NAMESPACE(WndDesign)