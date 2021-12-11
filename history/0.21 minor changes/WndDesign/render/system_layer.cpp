#include "system_layer.h"
#include "../window/wnd_base_impl.h"
#include "../system/win32_api.h"
#include "../style/style_helper.h"

BEGIN_NAMESPACE(WndDesign)


SystemWndLayer::SystemWndLayer(_WndBase_Impl& parent, Rect region, Ref<_WndBase_Impl*> wnd, WndStyle::RenderStyle style) :
	SingleWndLayer(parent, region, wnd, WndStyle::RenderStyle()),
	_target_resource_manager(*this, Win32::CreateWnd(this, region, style)),
	_system_target_tile(_tile_cache, region.size) {

	_system_target_tile.SetSystemTarget(&_target_resource_manager._system_target_container);

	// Initialize tile_cache with resourse manager.
	SetResourceManager(&_target_resource_manager);
}

SystemWndLayer::~SystemWndLayer() {
	// Clear tile_cache of child windows.
	SetResourceManager(nullptr);  

	// Clear the system tile before its destruction.
	_system_target_tile.SetSystemTarget(nullptr);  
	_target_resource_manager.DestroyResources();

	Win32::DestroyWnd(_target_resource_manager._hWnd);
}

void SystemWndLayer::RedrawChild(Rect region_to_update) const {
	SingleWndLayer::GetWnd()->Composite(region_to_update);  // SingleWndLayer::RedrawChild().
	const_cast<Tile&>(_system_target_tile).EndDraw();  // EndDraw here. (similar to CommitActiveTiles() of tile_cache)
}

void SystemWndLayer::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) {
	bounding_region = GetAccessibleRegion().Intersect(bounding_region);

	_system_target_tile.BeginDraw(bounding_region, vector_zero, false);

	for (auto& container : figure_queue) {
		// The Figure's entire region on the layer.
		Rect figure_region = container.GetRegion() + position_offset;
		if (figure_region.IsEmpty()) { continue; }

		// Draw on the system_target_tile.
		container.GetFigure().DrawOn(_system_target_tile, figure_region);
	}

	// The tile will EndDraw when RedrawChild finished.
}

bool SystemWndLayer::IsValid() const {
	return GetResourceManager() != nullptr;
}


void SystemWndLayer::SetRegion(Rect region) {
	Win32::MoveWnd(_target_resource_manager._hWnd, region);
}

void SystemWndLayer::StyleChanged() {
	Win32::SetWndStyle(_target_resource_manager._hWnd, _style);
}


void SystemWndLayer::SetCapture() {
	Win32::SetCapture(_target_resource_manager._hWnd);
}
void SystemWndLayer::ReleaseCapture() {
	Win32::ReleaseCapture();
}

void SystemWndLayer::SetFocus() {
	Win32::SetFocus(_target_resource_manager._hWnd);
}
void SystemWndLayer::ReleaseFocus() {
	Win32::ReleaseFocus();
}


void SystemWndLayer::RegionChanged(Rect region) {
	if (!IsValid()) { return; }
	if (_composite_region.size != region.size) {
		_target_resource_manager.Resize();
		_system_target_tile.SetTileSize(region.size);
	}
	SetCompositeRegion(region);

	// Overwrite the positioning styles.
	auto& style = GetWnd()->GetStyle();
	style.position.left(px(region.point.x)).top(px(region.point.y));
	style.position.right(position_auto).down(position_auto);
}

void SystemWndLayer::ReceiveSystemMessage(Msg msg, void* para) const {
	if (!IsValid()) { return; }
	GetWnd()->DispatchMessage(msg, para);
}

void SystemWndLayer::LoseCapture() { 
	if (!IsValid()) { return; }
	_parent.LoseCapture();
}

void SystemWndLayer::LoseFocus() {
	if (!IsValid()) { return; }
	_parent.LoseFocus();
}

std::pair<Size, Size> SystemWndLayer::CalculateMinMaxSize() {
	Size desktop_size = GetDesktopSize();
	auto& style = GetWnd()->GetStyle();
	return std::make_pair(CalculateMinSize(style, desktop_size), CalculateMaxSize(style, desktop_size));
}


END_NAMESPACE(WndDesign)