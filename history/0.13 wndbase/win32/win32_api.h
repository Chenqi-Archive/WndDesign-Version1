#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../style/style.h"

BEGIN_NAMESPACE(WndDesign)


using HANDLE = void*;

class SystemWndLayer;


BEGIN_NAMESPACE(Win32)

HANDLE CreateWnd(Ref SystemWndLayer* layer, Rect region, WndStyle::RenderStyle style);

void DestroyWnd(HANDLE hWnd);

void MoveWnd(HANDLE hWnd, Rect region);

void SetWndStyle(HANDLE hWnd, WndStyle::RenderStyle style);

int MessageLoop();

END_NAMESPACE(Win32)


END_NAMESPACE(WndDesign)