#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../style/style.h"
#include "../message/message.h"

BEGIN_NAMESPACE(WndDesign)


using HANDLE = void*;

class SystemWndLayer;


BEGIN_NAMESPACE(Win32)

HANDLE CreateWnd(Ref<SystemWndLayer*> layer, Rect region, WndStyle::RenderStyle style);
void DestroyWnd(HANDLE hWnd);

void MoveWnd(HANDLE hWnd, Rect region);
void SetWndStyle(HANDLE hWnd, WndStyle::RenderStyle style);

void SetCapture(HANDLE hWnd);
void ReleaseCapture(HANDLE hWnd);

// Set a timer that is not bound to a specific window.
// The timer will be dispatched in the system timer handler.
// If the timer is NULL, a new timer will be created and returned, 
//   else, the old timer will be updated with the new period.
HANDLE SetTimer(HANDLE timer, uint period);
void KillTimer(HANDLE timer);


int MessageLoop();

END_NAMESPACE(Win32)


END_NAMESPACE(WndDesign)