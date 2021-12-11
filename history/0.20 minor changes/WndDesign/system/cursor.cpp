#include "cursor.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API void SetCursor(Cursor cursor) {
    ::SetCursor(LoadCursorW(NULL, MAKEINTRESOURCE(cursor)));
}

WNDDESIGN_API Point GetCursorPoint() {
    POINT point;
    ::GetCursorPos(&point);
    return Point(point.x, point.y);
}


END_NAMESPACE(WndDesign)