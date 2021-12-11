#pragma once

#include "../common/core.h"
#include "../message/message.h"
#include "../style/wnd_style.h"
#include "../object/ObjectBase.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using WndDesignHelper::ObjectBase;
using std::unique_ptr;


AbstractInterface IWndBase {
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IWndBase() {}

public:
	struct Deleter { void operator()(IWndBase* wnd) { wnd->Destroy(); } };

	using StyleType = WndStyle;
	WNDDESIGN_API static unique_ptr<IWndBase, IWndBase::Deleter> 
		Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);

	virtual void Destroy() pure;
	virtual Ref<IWndBase*> GetParent() const pure;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	virtual void SetBackground(const Background& background) pure;
	virtual void SetOpacity(uchar opacity) pure;
	virtual const Size GetSize() const pure;

	// Set the region, returns true if the actual region has changed.
	virtual bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) pure;
	bool SetSize(ValueTag width, ValueTag height) { return SetRegion(width, height, position_auto, position_auto); }
	bool SetPosition(ValueTag left, ValueTag top) { return SetRegion(length_auto, length_auto, left, top); }

    // Get the point coordinate as in its layer, used for static child windows of scrollwnd.
	virtual const Point ConvertToLayerPoint(Point point) const pure;
	// Get the point coordinate as in parent window.
	virtual const Point ConvertToParentPoint(Point point) const pure; 
	// Get the point coordinate as in desktop.
	virtual const Point ConvertToDesktopPoint(Point point) const pure;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// [For Wnd, ScrollWnd, Table, and Desktop]

	// Add the child window.
	// Some window types do not support adding child windows. So you can check the 
	//   return value or get the wnd's parent to check if it was added successfully.
	virtual bool AddChild(Ref<IWndBase*> child_wnd) pure;
	// Remove the child window, but not destroy the window.
	virtual bool RemoveChild(Ref<IWndBase*> child_wnd) pure;


	///////////////////////////////////////////////////////////
	////                     Composite                     ////
	///////////////////////////////////////////////////////////
	// For controlling composition. (Rarely used)
	// If BlockComposition() is called, any content change of the window will not be committed to take effect, 
	//   until AllowComposition() or UpdateRegion() is explicitly called, or parent window starts drawing the window.
	// Often used to merge multiple drawing operations to improve efficiency.
	virtual void BlockComposition() pure;  
	virtual void AllowComposition() pure;
	virtual void UpdateRegion(Rect region) pure;


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
	// [For ScrollWnd and Table]

	// Get the scroll position. (relative to the base layer that will be drawn at left top corner)
	// Also use IWndBase::GetSize() to get the size of current region. Or you could directly call GetCurrentRegion().
	virtual const Point GetScrollPosition() const pure;
	// Set the scroll position. (The scroll region will be bounded within the entire region.)
	virtual void SetScrollPosition(Point point) pure;
	// Scroll the current region by vector.
	void ScrollView(Vector vector) { SetScrollPosition(GetScrollPosition() + vector); }

	// Get the entire accessible region.
	virtual const Rect GetEntireRegion() const pure;

	const Rect GetCurrentRegion() const { return Rect(GetScrollPosition(), GetSize()); }

	// Zoom in or out.
	//virtual void ScaleView(Point center, float ratio) pure;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;
};


END_NAMESPACE(WndDesign)