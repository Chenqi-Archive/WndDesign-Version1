#include "aero_snap.h"

#include <Windows.h>

BEGIN_NAMESPACE(WndDesignHelper)


void SystemWindowDraggingLeftDown(ObjectBase& object, Point point) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(object.GetWnd()));
	point = ConvertPointToSystemWndPoint(object.GetWnd(), point);
	union { struct { short x; short y; }; LPARAM lParam; }_point_conversion;
	_point_conversion.x = static_cast<short>(point.x);
	_point_conversion.y = static_cast<short>(point.y);
	SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, _point_conversion.lParam);
}


void SystemWindowBorderResizingLeftDown(ObjectBase& object, Point point, BorderPosition border_position) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(object.GetWnd()));
	point = ConvertPointToSystemWndPoint(object.GetWnd(), point);
	union { struct { short x; short y; }; LPARAM lParam; }_point_conversion;
	_point_conversion.x = static_cast<short>(point.x);
	_point_conversion.y = static_cast<short>(point.y);
	SendMessage(hWnd, WM_NCLBUTTONDOWN, static_cast<WPARAM>(border_position), _point_conversion.lParam);
}


END_NAMESPACE(WndDesignHelper)