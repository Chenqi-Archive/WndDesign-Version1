#pragma once

#include "scroll_wnd.h"
#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)

class _Scroll_Wnd_Impl : virtual public ScrollWnd, virtual public _Wnd_Impl {
	/////////////////////////////////////////////////////////
	////                      Style                      ////
	/////////////////////////////////////////////////////////
public:


	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
public:
	void SetCanvas(const Canvas& canvas) override;
	void Render() const override;


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
private:
	Ref ScrollBar* _scroll_bar;
private:
	// scroll: positive when the window moves down, negative when the window moves up.
	bool OnVerticalScroll(int scroll) {
		Point current_point = _base_layer.GetCurrentPoint();
		Point new_current_point = current_point + Vector{ 0, scroll };
		// Find the available region that the current point can lay in.
		Rect bounded_rect = _base_layer.GetAccessibleRegion() - _base_layer.GetCompositeRegion().size;
		// Round the point to the nearest point that is inside the accessible region.
		new_current_point = FindNearestPoint(bounded_rect, new_current_point);
		if (new_current_point == current_point) {
			// If has reached the border of the scrollable region, the scroll operation fails.
			return false;
		}
		_base_layer.SetCurrentPoint(new_current_point);
		return true;
	}

	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	void DispatchMessage(Msg msg, void* para) override;



	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
public:
	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;
};


END_NAMESPACE(WndDesign)