#pragma once

#include "../common/core.h"
#include "../message/message.h"

#include "../style/wnd_style.h"

BEGIN_NAMESPACE(WndDesign)


struct WndBase {
	WNDDESIGN_API static Alloc WndBase* Create(Ref WndBase* parent, const WndStyle& style, Handler handler);
	virtual ~WndBase() pure {}
	virtual void Destroy() pure;
	virtual Ref WndBase* GetParent() pure;
	

	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	virtual void SetRegion(LengthTag width, LengthTag height, LengthTag left, LengthTag top) pure;
	virtual void SetBackground(Color32 background) pure;
	virtual void SetOpacity(uchar opacity) pure;
	virtual const Size GetSize() const pure;
	void SetSize(LengthTag width, LengthTag height) { return SetRegion(width, height, position_auto, position_auto); }
	void SetPosition(LengthTag left, LengthTag top) { return SetRegion(length_auto, length_auto, left, top); }
	virtual const Point ConvertToParentPoint(Point point) const pure; // Get the point coordinate as in parent window.


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Add the child window.
	// You can check the return value or get the wnd's parent to check if it was added successfully.
	virtual bool AddChild(Ref WndBase* child_wnd) pure;
	// Remove the child window, but not destroy the window.
	virtual bool RemoveChild(Ref WndBase* child_wnd) pure;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
	virtual void SetHandler(Handler handler) pure;
	virtual void SetCapture() pure;
	virtual void ReleaseCapture() pure;
	virtual void SetFocus() pure;
	virtual void ReleaseFocus() pure;


	///////////////////////////////////////////////////////////
	////                     User Data                     ////
	///////////////////////////////////////////////////////////
	using Data = void*;
	virtual void SetUserData(Data data) pure;
	virtual Data GetUserData() const pure;
};


WNDDESIGN_API void SendMsg(Ref WndBase* wnd, Msg msg, Para para);


template<class WndType>
inline void SafeDestroy(WndType*& wnd) {
	if (wnd != nullptr) {
		wnd->Destroy();
		wnd = nullptr;
	}
}


END_NAMESPACE(WndDesign)