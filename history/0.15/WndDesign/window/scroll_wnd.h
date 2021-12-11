#pragma once

#include "wnd.h"

#include "../style/scroll_wnd_style.h"


BEGIN_NAMESPACE(WndDesign)

AbstractInterface IScrollWnd : virtual IWnd{
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IScrollWnd() {}

public:
	using StyleType = ScrollWndStyle;
	WNDDESIGN_API static Alloc<IScrollWnd*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Add a child window with positioning style. 
	virtual bool AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) pure;
	bool AddChildFixed(Ref<IWndBase*> child_wnd) { return AddChild(child_wnd, Positioning::Fixed, padding_null); }
	bool AddChildStatic(Ref<IWndBase*> child_wnd) { return AddChild(child_wnd, Positioning::Static, padding_null); }
	bool AddChildSticky(Ref<IWndBase*> child_wnd, Padding padding) { return AddChild(child_wnd, Positioning::Sticky, padding); }


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

	// Zoom in or out.
	//virtual void ScaleView(Point center, float ratio) pure;
};


END_NAMESPACE(WndDesign)