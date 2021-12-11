#include "win32.h"

#include "../window/wnd_base_impl.h"
#include "../render/system_layer.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Handle GetWndHandle(Ref<IWndBase*> wnd) {
	_WndBase_Impl* the_wnd = dynamic_cast<_WndBase_Impl*>(wnd);
	auto resource_manager = the_wnd->GetResourceManager();
	if (resource_manager == nullptr) { return NULL; }
	return resource_manager->GetSysWnd();
}


WNDDESIGN_API Point ConvertPointToSystemWndPoint(Ref<IWndBase*> wnd, Point point) {
	_WndBase_Impl* the_wnd = dynamic_cast<_WndBase_Impl*>(wnd);
	auto resource_manager = the_wnd->GetResourceManager();
	if (resource_manager == nullptr) { return point; }
	return resource_manager->GetLayer().ConvertToLayerPoint(wnd->ConvertToDesktopPoint(point));
}


END_NAMESPACE(WndDesign)