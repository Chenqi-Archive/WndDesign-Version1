#include "system_layer.h"
#include "../window/wnd_base_impl.h"
#include "../system/win32_api.h"

BEGIN_NAMESPACE(WndDesign)

SystemWndLayer::SystemWndLayer(_WndBase_Impl& parent, Rect region, Ref<_WndBase_Impl*> wnd,
							   WndStyle::RenderStyle style) :
	SingleWndLayer(parent, region, wnd, WndStyle::RenderStyle()),
	_target_resource_manager(Win32::CreateWnd(this, region, style)),
	_system_target_tile(_tile_cache, region.size) {

	_system_target_tile.SetSystemTarget(&_target_resource_manager._system_target_container);

	// Initialize tile_cache.
	SetResourceManager(&_target_resource_manager);
	RefreshCachedRegion(GetLayerVisibleRegion());
}

SystemWndLayer::~SystemWndLayer() {
	// Clear tile_cache of child windows.
	SetResourceManager(nullptr);  

	// Clear the system tile before its destruction.
	_system_target_tile.SetSystemTarget(nullptr);  
	_target_resource_manager.DestroyResources();

	Win32::DestroyWnd(_target_resource_manager._hWnd);
}

void SystemWndLayer::SetRegion(Rect region) {
	Win32::MoveWnd(_target_resource_manager._hWnd, region);
	if (_composite_region.size != region.size) {
		_target_resource_manager.Resize();
		_system_target_tile.SetTileSize(region.size);
	}
	SetCompositeRegion(region);
}

void SystemWndLayer::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) {
	bounding_region = GetAccessibleRegion().Intersect(bounding_region);

	_system_target_tile.BeginDraw(bounding_region, vector_zero, false);

	for (uint pos = 0; pos < figure_queue.Size(); ++pos) {
		const FigureContainer& container = figure_queue.Get(pos);

		// The Figure's entire region on the layer.
		Rect figure_region = container.GetRegion() + position_offset;
		if (figure_region.IsEmpty()) { continue; }

		// Draw on the system_target_tile.
		container.GetFigure().DrawOn(_system_target_tile, figure_region);
	}

	_system_target_tile.EndDraw();
}

void SystemWndLayer::ReceiveSystemMessage(Msg msg, void* para) const {
	GetWnd()->DispatchMessage(msg, para);
}

void SystemWndLayer::SetCapture() {
	Win32::SetCapture(_target_resource_manager._hWnd);
}
void SystemWndLayer::ReleaseCapture() {
	Win32::ReleaseCapture();
}
void SystemWndLayer::LoseCapture() { 
	GetWnd()->LoseCapture(); 
}

void SystemWndLayer::SetFocus() {
	Win32::SetFocus(_target_resource_manager._hWnd);
}
void SystemWndLayer::ReleaseFocus() {
	Win32::ReleaseFocus();
}
void SystemWndLayer::LoseFocus() {
	GetWnd()->LoseFocus();
}


END_NAMESPACE(WndDesign)