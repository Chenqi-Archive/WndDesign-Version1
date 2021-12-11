#include "layer.h"

#include "../window/wnd_base_impl.h"
#include "../style/style_helper.h"

BEGIN_NAMESPACE(WndDesign)


Layer::Layer(_WndBase_Impl& parent, Rect composite_region, bool is_redirected,
			 Rect accessible_region, Point current_point, Color32 background, WndStyle::RenderStyle style):
	_parent(parent),
	_composite_region(composite_region),
	_current_point(current_point),
	_is_redirected(is_redirected),
	_style(style),
	_tile_cache(AcquireResourceManager(),
				background,
				accessible_region,
				GetLayerVisibleRegion(),
				CachePolicy(CachePolicy::ScaleVisibleRegion, 0, 2.0)),
	_is_redrawing_child(false) {
}

void Layer::RefreshCachedRegion(Rect visible_region) {
	// If the new visible region falls out of the cached region, reset the cached region, and redraw child windows.
	Rect old_cached_region = _tile_cache.GetCachedRegion();
	if (!old_cached_region.Contains(visible_region)) {
	#pragma message("Todo: Offset Hint")
		_tile_cache.SetCachedRegion(visible_region);

		Rect new_cached_region = _tile_cache.GetCachedRegion();

		// Refresh the child window's visible region.
		RefreshChildVisibleRegion(new_cached_region);

		// Set the tile mask to be the intersection of old and new cached regions, so these old tiles
		//   will not be redrawed when child windows redraw only on new tiles.
		_tile_cache.SetTileMask(old_cached_region.Intersect(new_cached_region));

		// Redraw all child windows overlapped with the region.
		RegionUpdated(new_cached_region, false);

		// Clear the tile mask.
		_tile_cache.ClearTileMask();
	}
}

Rect Layer::GetLayerVisibleRegion() const {
	return _parent.GetVisibleRegion().Intersect(_composite_region) - (_composite_region.point - _current_point);
}

Rect Layer::GetCachedRegion() const {
	if (_is_redirected) {
		return _parent.GetVisibleRegion();
	} else {
		return _tile_cache.GetCachedRegion();
	}
}

void Layer::RegionUpdated(Rect region_to_update, bool commit_immediately) const {
	if (_is_redrawing_child) { Error(); }

	region_to_update = GetCachedRegion().Intersect(region_to_update);
	if (region_to_update.IsEmpty()) { return; }

	if (!_is_redirected) { 
		_is_redrawing_child = true;
		RedrawChild(region_to_update); 
		_is_redrawing_child = false;
		_tile_cache.CommitActiveTiles();
	}
	// If the layer is redirected, it will redraw child at parent's composition time.

	if (commit_immediately) { 
		_parent.LayerUpdated(this, region_to_update); 
	}
}

void Layer::CompositeChild(Rect region_to_update) const {
	if (!_is_redirected) { return; }
	_is_redrawing_child = true;
	RedrawChild(region_to_update);
	_is_redrawing_child = false;
}

void Layer::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) {
	if (!_is_redrawing_child) { Error(L"Please call RegionUpdated instead of calling Composite!"); }

	if (_is_redirected) {
		// The layer is redirected to the parent's canvas, so it must be a base layer.
		// The positioning of the layer is the same as the parent's canvas, so there is no need to do conversion.
		_parent.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	} else {
		_tile_cache.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}
}

Ref<TargetResourceManager*> Layer::AcquireResourceManager() {
	return _parent.GetResourceManager();
}



SingleWndLayer::SingleWndLayer(_WndBase_Impl& parent, Rect composite_region, Ref<_WndBase_Impl*> wnd,
							   WndStyle::RenderStyle style, Rect relative_region, const Padding& padding) :
	Layer(parent, composite_region, false,
		  { point_zero, composite_region.size }, point_zero,
		  color_transparent, style),
	_wnd(wnd),
	_relative_region(relative_region),
	_padding(padding) {
	_wnd->SetCanvas(Canvas(this, { point_zero, composite_region.size }));
}

SingleWndLayer::~SingleWndLayer() {
	_wnd->SetCanvas(Canvas());
}

void SingleWndLayer::SetCompositeRegion(const Rect& composite_region) {
	bool size_changed = (_composite_region.size != composite_region.size);
	_composite_region = composite_region;
	if (size_changed) {
		_tile_cache.SetAccessibleRegion({ point_zero, _composite_region.size }, GetLayerVisibleRegion());
		_wnd->SetCanvas(Canvas(this, { point_zero, _composite_region.size }));
	} else {
		// The size has not changed, just refresh the visible region for tile cache.
		RefreshCachedRegion(GetLayerVisibleRegion());
	}
}

void SingleWndLayer::RefreshChildVisibleRegion(Rect visible_region) const {
	_wnd->RefreshVisibleRegion(visible_region);
}

void SingleWndLayer::RedrawChild(Rect region_to_update) const {
	_wnd->Composite(region_to_update);
}

void SingleWndLayer::SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) {
	_wnd->ResetLayerResourceManager(resource_manager);
}

Ref<_WndBase_Impl*> SingleWndLayer::MouseHitTest(Point point) const {
	if (_wnd->MouseHitTest(point)) {
		return _wnd;
	}
	return nullptr;
}



bool MultipleWndLayer::SetCurrentPoint(Point point) {
	point = BoundPointInRegion(point, ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
	if (point == GetCurrentPoint()) { return false; }
	_current_point = point;
	RefreshCachedRegion(GetLayerVisibleRegion());
	return true;
}

MultipleWndLayer::MultipleWndLayer(_WndBase_Impl& parent) :
	Layer(parent, region_empty, true,
		  region_empty, point_zero, parent.GetStyle().background),
	_entire_region() {
}

MultipleWndLayer::MultipleWndLayer(_WndBase_Impl& parent, ScrollWndStyle::RegionStyle entire_region):
	Layer(parent, region_empty, false,
		  CalculateAccessibleRegion(entire_region, size_min), point_zero, parent.GetStyle().background),
	_entire_region(entire_region) {
}

MultipleWndLayer::~MultipleWndLayer() {
	for (auto& it : _wnds) {
		it.wnd->SetCanvas(Canvas());
	}
}

void MultipleWndLayer::SetUnredirected() {
	if (!_is_redirected) { return; }
	_is_redirected = false;
	RegionUpdated(region_infinite, false);
	// The tile_cached has been initialized even if the layer was redirected, 
	//   but it has no active tiles, until a new figure is actually drawn.
}

bool MultipleWndLayer::SetCompositeSize(Size parent_size) {
	_composite_region.size = parent_size;
	Rect new_region = CalculateAccessibleRegion(_entire_region, parent_size);
	if (new_region == GetAccessibleRegion()) { // For scrollwnd.
		return false;
	}
	// If the accessible region has changed, reset the current point, 
	//   and all child windows will be redrawn.
	_tile_cache.SetAccessibleRegion(new_region, GetLayerVisibleRegion());
	_current_point = BoundPointInRegion(_current_point, 
										ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
	return true;
}

void MultipleWndLayer::RefreshChildVisibleRegion(Rect visible_region) const {
	// Draw all related child windows.
	for (auto& it : _wnds) {
		Rect intersected_region = visible_region.Intersect(it.region);
		if (intersected_region.IsEmpty()) { continue; }
		Rect region_on_child = intersected_region - (it.region.point - point_zero);
		it.wnd->RefreshVisibleRegion(region_on_child);
	}
}

void MultipleWndLayer::RedrawChild(Rect region_to_update) const {
	// Redraw the background.
	if (_is_redirected) {
		FigureQueue figure_queue;
		figure_queue.Push(new Color(_parent.GetStyle().background), region_to_update);
		_parent.DrawFigureQueue(figure_queue, region_to_update, vector_zero);
	} else {
		const_cast<TileCache&>(_tile_cache).DrawBackground(region_to_update);
	}

	// Draw all related child windows.
	for (auto& it : _wnds) {
		Rect intersected_region = region_to_update.Intersect(it.region);
		if (intersected_region.IsEmpty()) { continue; }
		Rect region_on_child = intersected_region - (it.region.point - point_zero);
		it.wnd->Composite(region_on_child);
	}
}

void MultipleWndLayer::SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) {
	for (auto& it : _wnds) { it.wnd->ResetLayerResourceManager(resource_manager); }
}

void MultipleWndLayer::AddWnd(Ref<_WndBase_Impl*> wnd, Rect region) {
	_wnds.push_back({ wnd, region });
	wnd->SetCanvas(Canvas(this, region));
	// RegionUpdated() will should be called by parent later.
}

void MultipleWndLayer::RemoveWnd(Ref<_WndBase_Impl*> wnd) {
	for (auto it = _wnds.begin(); it != _wnds.end(); ++it) {
		if (it->wnd != wnd) { continue; }
		wnd->SetCanvas(Canvas());
		_wnds.erase(it);
		// RegionUpdated() will should be called by parent later.
		return;
	}
	Error();  // Control should never reach here!
}

void MultipleWndLayer::MoveWnd(Ref<_WndBase_Impl*> wnd, Rect new_region) {
	for (auto it = _wnds.begin(); it != _wnds.end(); ++it) {
		if (it->wnd != wnd) { continue; }
		wnd->SetCanvas(Canvas(this, new_region));
		it->region = new_region;
		// RegionUpdated() will should be called by parent later.
		return;
	}
	Error();  // Control should never reach here!
}

void MultipleWndLayer::ClearChild() {
	_wnds.clear();
}

void MultipleWndLayer::ResetChildRegion() {
	Rect accessible_region = GetAccessibleRegion();
	Rect region_to_update = region_empty;
	for (auto& it : _wnds) {
		Rect region = CalculateActualRegion(it.wnd->GetStyle(), accessible_region);
		if (region != it.region) {
			region_to_update = region_to_update.Union(region.Union(it.region));
			it.wnd->SetCanvas(Canvas(this, region));
			it.region = region;
		}
	}
	RegionUpdated(region_to_update, false);
}

Ref<_WndBase_Impl*> MultipleWndLayer::MouseHitTest(Point point) const {
	for (auto it = _wnds.begin(); it != _wnds.end(); ++it) {
		if (!it->region.Contains(point)) { continue; }
		Point child_point = point - (it->region.point - point_zero);
		if (it->wnd->MouseHitTest(child_point)) {
			return it->wnd;
		}
	}
	return nullptr;
}


END_NAMESPACE(WndDesign)