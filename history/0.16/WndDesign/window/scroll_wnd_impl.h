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
private:
	void SizeChanged() override;
public:
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
	// The child window's region style has changed, update the new region.
	// The positioning style should not be changed.
	bool UpdateChildRegion(Ref<IWndBase*> child_wnd) override;
};


END_NAMESPACE(WndDesign)