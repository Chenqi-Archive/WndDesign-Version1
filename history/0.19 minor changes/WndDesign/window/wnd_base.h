#pragma once

#include "../common/core.h"
#include "../message/message.h"
#include "../style/wnd_style.h"
#include "../object/ObjectBase.h"

#include <memory>


BEGIN_NAMESPACE(WndDesign)

using WndDesignHelper::ObjectBase;
using std::unique_ptr;


AbstractInterface IWndBase{
	using StyleType = WndStyle;
	WNDDESIGN_API static unique_ptr<IWndBase> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);

	virtual ~IWndBase() pure {}
	virtual Ref<IWndBase*> GetParent() const pure;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	virtual WndStyle& GetStyle() const pure;
	virtual bool RegionStyleUpdated() pure;//(retruns true if size changed) // SizeStyle, PositionStyle -> parent window
	virtual void RenderStyleUpdated() pure;									// RenderStyle				-> parent window
	void ContentStyleUpdated() { UpdateRegion(region_infinite); }			// BorderStyle, Background  -> myself

	// Some helper functions for setting specific styles.
	bool SetSize(uint width, uint height) { GetStyle().size.normal(px(width), px(height)); return RegionStyleUpdated(); }
	bool SetPosition(int left, int top) { GetStyle().position.left(px(left)).top(px(top)); return RegionStyleUpdated(); }

	void SetOpacity(uchar opacity) { GetStyle().render._opacity = opacity; RenderStyleUpdated(); }

	void SetBorder(uint width, Color color) { GetStyle().border.width(px(width)).color(color); ContentStyleUpdated(); }
	void SetBackground(const Background& background) { GetStyle().background = background; ContentStyleUpdated(); }


	virtual const Size GetSize() const pure;

	// Get the point coordinate as in its layer, used for static child windows of scrollwnd.
	virtual const Point ConvertToLayerPoint(Point point) const pure;
	// Get the point coordinate as in parent window.
	virtual const Point ConvertToParentPoint(Point point) const pure;
	// Get the point coordinate as in desktop.
	virtual const Point ConvertToDesktopPoint(Point point) const pure;

	const Rect GetRegionOnParent() const { return Rect(ConvertToParentPoint(point_zero), GetSize()); }


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

	// Zoom in or out.
	//virtual void ScaleView(Point center, float ratio) pure;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;

	// Block message from sending to child so that all messages are handled by myself.
	virtual void BlockChildMsg() pure;
};


END_NAMESPACE(WndDesign)