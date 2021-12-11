#include "desktop_impl.h"

#include "../win32/win32_api.h"

BEGIN_NAMESPACE(WndDesign)


SystemWndLayer::SystemWndLayer() :
	SingleWndLayer(),
	_target_resource_manager(),
	_system_target_tile(nullptr) {
}

SystemWndLayer::~SystemWndLayer() {
	Clear();
}

void SystemWndLayer::Init(Ref _WndBase_Impl* parent, Rect region, Ref _WndBase_Impl* wnd, WndStyle::RenderStyle style) {

	HANDLE hWnd = Win32::CreateWnd(this, region, style);

	_target_resource_manager.Init(hWnd);
	_system_target_tile.SetSystemTarget(&_tile_cache,
										&_target_resource_manager._system_target_container);
	// It's a bit cheating here, the system tile borrows the layer's tile cache for acquiring drawing resources.

	SingleWndLayer::Init(parent, region, wnd, WndStyle::RenderStyle());
}

void SystemWndLayer::Clear() {
	SingleWndLayer::Clear();

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


_Desktop_Impl::_Desktop_Impl():
	_Wnd_Impl(nullptr, WndStyle(), nullptr) {}

_Desktop_Impl::~_Desktop_Impl() {
	Destroy();
}

void _Desktop_Impl::Destroy() {
	for (auto it_layer = _layers.begin(); it_layer != _layers.end(); ) {
		_layers.erase(it_layer++);
	}
	// Message loop will automatically quit after destroying all layers.
}

void _Desktop_Impl::Composite(Rect region_to_update, const Ref Layer* layer) const {
	const Ref SystemWndLayer* the_layer = static_cast<const Ref SystemWndLayer*>(layer);
	if (the_layer == nullptr) { return; }
	the_layer->RegionUpdated(region_to_update);
}

void _Desktop_Impl::LayerUpdated(const Ref Layer* layer, Rect region) {
	Composite(region, layer);
}

bool _Desktop_Impl::AddChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	Rect region = child->GetStyle().CalculateActualRegion(GetDesktopSize());
	_layers.push_back(SystemWndLayer()); SystemWndLayer& layer = _layers.back();
	layer.Init(this, region, child, child->GetStyle().render);
	_child_wnds.push_back({ child, region, &layer });

	return true;
}

bool _Desktop_Impl::RemoveChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
	if (it == _child_wnds.end()) { return false; }

	for (auto it_layer = _layers.begin(); it_layer != _layers.end(); ++it_layer) {
		if (it->layer == &*it_layer) {
			_layers.erase(it_layer);
			return true;
		}
	}

	return false;
}

void _Desktop_Impl::MoveChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
	if (it == _child_wnds.end()) { return; }

	// Calculate the new region.
	const WndStyle& style = child->GetStyle();
	Rect new_region = style.CalculateActualRegion(_canvas.GetSize());
	if (new_region == it->region) { return; }

	it->region = new_region;
	static_cast<SystemWndLayer*>(it->layer)->SetRegion(new_region);
}

void _Desktop_Impl::SetChildRenderStyle(Ref WndBase* child_wnd) {

}

int _Desktop_Impl::EnterMessageLoop() {
	if (_child_wnds.empty()) { return 0; }
	return Win32::MessageLoop();
}





Ref WNDDESIGN_API Desktop* desktop = nullptr;

Ref _Desktop_Impl* GetDesktop() {
	static _Desktop_Impl _desktop_object;
	return &_desktop_object;
}


END_NAMESPACE(WndDesign)