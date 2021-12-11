#pragma once

#include "wnd.h"

#include "../style/scroll_wnd_style.h"


BEGIN_NAMESPACE(WndDesign)

struct ScrollWnd : virtual Wnd {
	WNDDESIGN_API static Alloc ScrollWnd* Create(Ref Wnd* parent, const Ref ScrollWndStyle* style, Handler handler = nullptr);

	// Add the child window at the point. If the position can be ca
	virtual bool AddChild(Ref WndBase* child_wnd, Point point = point_zero) pure;
	// Remove the child window, but not destroy the window.
	virtual bool RemoveChild(Ref WndBase* child_wnd) pure;


	//// Set the current display position.
	//virtual void SetPosition(const Point& point) pure;
	//
	//// Move the current display position by the vector.
	//virtual void MovePosition(const Vector& vector) pure;

	//// Zoom in or out.
	////virtual void ScaleView(Point center, float ratio) pure;
};


END_NAMESPACE(WndDesign)
