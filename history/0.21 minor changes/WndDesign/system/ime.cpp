#include "ime.h"

#include "win32.h"
#include "win32_ime_input.h"

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API void EnableIME(Ref<IWndBase*> wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().EnableIME(hWnd);
}

WNDDESIGN_API void DisableIME(Ref<IWndBase*> wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().DisableIME(hWnd);
}

WNDDESIGN_API void CancelIME(Ref<IWndBase*> wnd) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	ImeInput::Get().CancelIME(hWnd);
}

WNDDESIGN_API void MoveImeWindow(Ref<IWndBase*> wnd, Rect caret_region) {
	HWND hWnd = static_cast<HWND>(GetWndHandle(wnd));
	if (hWnd == NULL) { return; }
	caret_region.point = ConvertPointToSystemWndPoint(wnd, caret_region.point);
	ImeInput::Get().UpdateCaretRect(hWnd, caret_region);
}


END_NAMESPACE(WndDesign)