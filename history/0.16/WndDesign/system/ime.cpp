#include "ime.h"

#include "../window/wnd_base_impl.h"
#include "../render/system_layer.h"

#include "win32_ime_input.h"

BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)

inline HWND GetWndHandle(Ref<IWndBase*> wnd) {
	_WndBase_Impl* the_wnd = dynamic_cast<_WndBase_Impl*>(wnd);
	auto resource_manager = the_wnd->GetResourceManager();
	if (resource_manager == nullptr) { return NULL; }
	return static_cast<HWND>(resource_manager->GetSysWnd());
}

END_NAMESPACE(Anonymous)


WNDDESIGN_API void EnableIME(Ref<IWndBase*> wnd) {
	HWND hWnd = GetWndHandle(wnd);
	if (hWnd == NULL) { return; }
	ImeInput::Get().EnableIME(hWnd);
}

WNDDESIGN_API void DisableIME(Ref<IWndBase*> wnd) {
	HWND hWnd = GetWndHandle(wnd);
	if (hWnd == NULL) { return; }
	ImeInput::Get().DisableIME(hWnd);
}

WNDDESIGN_API void CancelIME(Ref<IWndBase*> wnd) {
	HWND hWnd = GetWndHandle(wnd);
	if (hWnd == NULL) { return; }
	ImeInput::Get().CancelIME(hWnd);
}

WNDDESIGN_API void MoveCaret(Ref<IWndBase*> wnd, Rect caret_region) {
	HWND hWnd = GetWndHandle(wnd);
	if (hWnd == NULL) { return; }
	caret_region.point = wnd->ConvertToDesktopPoint(caret_region.point);
	ImeInput::Get().UpdateCaretRect(GetWndHandle(wnd), caret_region);
}



END_NAMESPACE(WndDesign)
