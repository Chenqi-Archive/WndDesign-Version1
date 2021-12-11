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


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
public:
	bool AddChild(Ref<IWndBase*> child_wnd) override { return AddChildFixed(child_wnd); }
	bool AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) override;
	// The child window's region style has changed, update the new region.
	// The positioning style should not be changed.
	bool UpdateChildRegion(Ref<_WndBase_Impl*> child) override;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	// Recalculate layer regions and reset canvas for child windows.
	void SizeChanged() override;
public:
	const Rect CalculateStickyLayerRegion(Rect relative_region, Padding padding);


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
public:
	const Point GetScrollPosition() const override { return _base_layer.GetCurrentPoint(); }
	void SetScrollPosition(Point point) override;
	const Rect GetEntireRegion() const override { return _base_layer.GetAccessibleRegion(); }

	void SetEntrieRegion(ScrollWndStyle::RegionStyle entire_region);
};


END_NAMESPACE(WndDesign)