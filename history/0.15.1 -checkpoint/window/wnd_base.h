#pragma once

#include "../common/core.h"
#include "../message/message.h"

#include "../style/wnd_style.h"


BEGIN_NAMESPACE(WndDesignHelper)
struct ObjectBase;
END_NAMESPACE(WndDesignHelper)


BEGIN_NAMESPACE(WndDesign)

using WndDesignHelper::ObjectBase;


AbstractInterface IWndBase {
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IWndBase() {}

public:
	using StyleType = WndStyle;

	WNDDESIGN_API static Alloc<IWndBase*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object);
	virtual void Destroy() pure;
	virtual Ref<IWndBase*> GetParent() pure;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	virtual void SetBackground(Color background) pure;
	virtual void SetOpacity(uchar opacity) pure;
	virtual const Size GetSize() const pure;

	// Set the region, returns true if the actual region has changed.
	virtual bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) pure;
	bool SetSize(ValueTag width, ValueTag height) { return SetRegion(width, height, position_auto, position_auto); }
	bool SetPosition(ValueTag left, ValueTag top) { return SetRegion(length_auto, length_auto, left, top); }

    // Get the point coordinate as in its layer, used for child windows of scrollwnd.
	virtual const Point ConvertToLayerPoint(Point point) const pure;
	// Get the point coordinate as in parent window.
	virtual const Point ConvertToParentPoint(Point point) const pure; 
	// Get the point coordinate as in desktop.
	virtual const Point ConvertToDesktopPoint(Point point) const pure;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Add the child window.
	// You can check the return value or get the wnd's parent to check if it was added successfully.
	virtual bool AddChild(Ref<IWndBase*> child_wnd) pure;
	// Remove the child window, but not destroy the window.
	virtual bool RemoveChild(Ref<IWndBase*> child_wnd) pure;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetObject(Ref<ObjectBase*> object);

	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;
	virtual bool DispatchMessage(Msg msg, Para para) pure;


	///////////////////////////////////////////////////////////
	////                     User Data                     ////
	///////////////////////////////////////////////////////////
	using Data = Ref<void*>;
	virtual void SetUserData(Data data) pure;
	virtual Data GetUserData() const pure;
};


inline void SendMsg(Ref<IWndBase*> wnd, Msg msg, Para para) {
	wnd->DispatchMessage(msg, para);
}


END_NAMESPACE(WndDesign)