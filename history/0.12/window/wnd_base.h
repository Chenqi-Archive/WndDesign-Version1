#pragma once

#include "../common/core.h"
#include "../common/msg.h"

#include "../style/wnd_style.h"

BEGIN_NAMESPACE(WndDesign)

// The base class for all window objects.
// Only with background, has no subwindow.

struct Wnd;

struct WndBase {
	WNDDESIGN_API static Alloc WndBase* Create(Ref Wnd* parent, const WndStyle& style, Handler handler);

	virtual ~WndBase() pure {}
	virtual void SetHandler(Handler handler) pure;
	
	//////  Style  //////
	virtual void SetStyle(const WndStyle& style) pure;  // This opeartion is expensive. Try not to use it.
	virtual void SetSize(WndStyle::SizeStyle size_style) pure;
	virtual void SetPosition(WndStyle::PositionStyle position_style) pure;
	virtual void SetBackground(const Ref Texture* background) pure;
	virtual void SetOpacity(uchar opacity) pure;

	virtual Point ConvertToParentPoint(Point point) pure;  // Get the point coordinate as in parent window.
};

// All sub windows will be destroyed.
WNDDESIGN_API void DestroyWnd(Alloc WndBase* wnd);

WNDDESIGN_API void SendMsg(Ref WndBase* wnd, Msg msg, Para para);


END_NAMESPACE(WndDesign)
