#pragma once

#include "wnd.h"

#include "../style/scroll_wnd_style.h"


BEGIN_NAMESPACE(WndDesign)

struct ScrollWnd : virtual Wnd {
	WNDDESIGN_API static Alloc ScrollWnd* Create(Ref WndBase* parent, const ScrollWndStyle& style, Handler handler = nullptr);


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Add a child window with positioning style. 
	virtual bool AddChild(Ref WndBase* child_wnd, Positioning positioning, Padding padding) pure;
	bool AddChildFixed(Ref WndBase* child_wnd) { return AddChild(child_wnd, Positioning::Fixed, padding_null); }
	bool AddChildStatic(Ref WndBase* child_wnd) { return AddChild(child_wnd, Positioning::Static, padding_null); }
	bool AddChildSticky(Ref WndBase* child_wnd, Padding padding) { return AddChild(child_wnd, Positioning::Sticky, padding); }


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
	// Get the visible position. (The position on the base layer that will be drawn at left top)
	virtual const Point GetPosition() const pure;
	// Set the visible position.
	virtual void SetPosition(Point point) pure;
	// Move the visible region by vector.
	void MoveView(Vector vector) { SetPosition(GetPosition() + vector); }

	// Get the entire region.
	virtual const Rect GetEntireRegion() const pure;

	// Set the scroll bar.
	virtual void SetScrollBar(Ref ScrollBar* scroll_bar) pure;

	// Zoom in or out.
	//virtual void ScaleView(Point center, float ratio) pure;
};


END_NAMESPACE(WndDesign)
