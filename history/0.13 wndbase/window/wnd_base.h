#pragma once

#include "../common/core.h"
#include "../common/msg.h"

#include "../style/wnd_style.h"

BEGIN_NAMESPACE(WndDesign)

// The base class for all window objects.
// Only with background, has no subwindow.

struct Wnd;
struct WndStyle;

struct WndBase {
	WNDDESIGN_API static Alloc WndBase* Create(Ref Wnd* parent, const WndStyle& style, Handler handler);

	virtual ~WndBase() pure {}
	virtual void Destroy() pure;

	virtual void SetHandler(Handler handler) pure;
	
	//////  Style  //////
	virtual void SetSize(LengthTag width, LengthTag height) pure;   // This operation changes the normal size only.
	virtual void SetPosition(LengthTag left, LengthTag top) pure;
	virtual void SetBackground(Color32 background) pure;
	virtual void SetOpacity(uchar opacity) pure;

	virtual Point ConvertToParentPoint(Point point) pure;  // Get the point coordinate as in parent window.

	// One can store data in a pointer's size with the window for convenience.
	using Data = void*;
	virtual void SetUserData(Data data) pure;
	virtual Data GetUserData() const pure;
};


WNDDESIGN_API void SendMsg(Ref WndBase* wnd, Msg msg, Para para);


END_NAMESPACE(WndDesign)
