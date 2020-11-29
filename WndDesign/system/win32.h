#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

BEGIN_NAMESPACE(WndDesign)


using Handle = void*;

struct IWndBase;


// Get the win32 HWND of wnd.
// Returns 0 if wnd is not the descendant of Desktop.
// You can convert it to HWND safely by static_cast.
WNDDESIGN_API Handle GetWndHandle(Ref<IWndBase*> wnd);


// Convert a point on wnd to the point on its cooresponding win32 system window.
WNDDESIGN_API Point ConvertPointToSystemWndPoint(Ref<IWndBase*> wnd, Point point);


END_NAMESPACE(WndDesign)