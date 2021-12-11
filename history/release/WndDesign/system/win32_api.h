#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../style/style.h"
#include "../message/message.h"

BEGIN_NAMESPACE(WndDesign)


using Handle = void*;

class SystemWndLayer;


BEGIN_NAMESPACE(Win32)

Handle CreateWnd(Ref<SystemWndLayer*> layer, Rect region, WndStyle::RenderStyle style);
void DestroyWnd(Handle hWnd);

void MoveWnd(Handle hWnd, Rect region);
void SetWndStyle(Handle hWnd, WndStyle::RenderStyle style);

void SetCapture(Handle hWnd);
void ReleaseCapture();

void SetFocus(Handle hWnd);
void ReleaseFocus();


int MessageLoop();



END_NAMESPACE(Win32)


END_NAMESPACE(WndDesign)