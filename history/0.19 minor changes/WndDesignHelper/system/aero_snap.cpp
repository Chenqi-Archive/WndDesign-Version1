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
//
//void SystemWindowDraggingMouseMove(ObjectBase& object, Point point) {
//	HWND hWnd = static_cast<HWND>(GetWndHandle(object.GetWnd()));
//	point = ConvertPointToSystemWndPoint(object.GetWnd(), point);
//	union { struct { short x; short y; }; LPARAM lParam; }_point_conversion;
//	_point_conversion.x = static_cast<short>(point.x);
//	_point_conversion.y = static_cast<short>(point.y);
//	SendMessage(hWnd, WM_NCMOUSEMOVE, HTCAPTION, _point_conversion.lParam);
//}
//
//void SystemWindowDraggingLeftUp(ObjectBase& object, Point point) {
//	HWND hWnd = static_cast<HWND>(GetWndHandle(object.GetWnd()));
//	point = ConvertPointToSystemWndPoint(object.GetWnd(), point);
//	union { struct { short x; short y; }; LPARAM lParam; }_point_conversion;
//	_point_conversion.x = static_cast<short>(point.x);
//	_point_conversion.y = static_cast<short>(point.y);
//	SendMessage(hWnd, WM_NCLBUTTONUP, HTCAPTION, _point_conversion.lParam);
//}


END_NAMESPACE(WndDesignHelper)