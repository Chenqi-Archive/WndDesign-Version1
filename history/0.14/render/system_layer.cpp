#include "system_layer.h"
#include "../window/wnd_base_impl.h"
#include "../win32/win32_api.h"

BEGIN_NAMESPACE(WndDesign)

SystemWndLayer::SystemWndLayer(Ref _WndBase_Impl* parent, Rect region, Ref _WndBase_Impl* wnd,
							   WndStyle::RenderStyle style) :
	SingleWndLayer(parent, region, wnd, WndStyle::RenderStyle()),
	_target_resource_manager(Win32::CreateWnd(this, region, style)),
	_system_target_tile(nullptr) {

	// Initialize tile_cache.
	SetResourceManager(&_target_resource_manager);
	RefreshCachedRegion(GetLayerVisibleRegion());

	// It's a bit cheating here, the system tile borrows the layer's tile cache to use drawing resources.
	_system_target_tile.SetSystemTarget(&_tile_cache, &_target_resource_manager._system_target_container);
}

SystemWndLayer::~SystemWndLayer() {
	_system_target_tile.SetSystemTarget(nullptr, nullptr);  // In case the target will return the target.
	_target_resource_manager.DestroyResources();
	Win32::DestroyWnd(_target_resource_manager._hWnd);
}

void SystemWndLayer::SetRegion(Rect region) {
	Win32::MoveWnd(_target_resource_manager._hWnd, region);
	SetCompositeRegion(region);
	if (_composite_region.size != region.size) {
		_target_resource_manager.Resize();
	}
}

void SystemWndLayer::ReceiveSystemMessage(Msg msg, void* para) const {
	GetWnd()->DispatchMessage(msg, para);
}

void SystemWndLayer::SetCapture() {
	Win32::SetCapture(_target_resource_manager._hWnd);
}

void SystemWndLayer::ReleaseCapture() {
	Win32::ReleaseCapture(_target_resource_manager._hWnd);
}

void SystemWndLayer::LoseCapture() { GetWnd()->LoseCapture(); }

END_NAMESPACE(WndDesign)