#include "layer.h"

#include "../window/wnd_base_impl.h"


BEGIN_NAMESPACE(WndDesign)


Layer::Layer() :
	_parent(nullptr),
	_composite_region(region_empty),
	_current_point(point_zero),
	_is_redirected(false),
	_style(),
	_visible_region(region_empty),
	_tile_cache(),
	_is_redrawing_child(false) {
}

Layer::~Layer() { 
	Clear(); 
}

void Layer::Init(Ref _WndBase_Impl* parent, Rect composite_region, bool is_redirected,
				 Rect accessible_region, Point current_point,
				 Color32 background, WndStyle::RenderStyle style) {
	if (!IsEmpty()) { Clear(); }

	_parent = parent;
	_composite_region = composite_region;
	_current_point = current_point;

	_style = style;

	// For redirected layer, the positioning attributes are the same as the parent's canvas.
	_is_redirected = is_redirected;

	Rect parent_visible_region = _parent->GetVisibleRegion();
	_visible_region = parent_visible_region.Intersect(_composite_region) - (_composite_region.point - _current_point);

	// There's no checking if the visible region falls in the current region.

	// Redirected layers has no tile cache.
	if (!_is_redirected) {
		_tile_cache.Init(AcquireResourceManager(),
						 CalculateTileSize(_composite_region.size, _style._width_change, _style._height_change),
						 background,
						 accessible_region,
						 _visible_region,
						 2.0);
	}

	_is_redrawing_child = false;
}

void Layer::Clear() {
	_parent = nullptr;
	_composite_region = region_empty;
	_current_point = point_zero;
	_is_redirected = false;
	_style = WndStyle::RenderStyle();
	_visible_region = region_empty;
	_tile_cache.Clear();
	_is_redrawing_child = false;
}

void Layer::SetStyle(WndStyle::RenderStyle style) {
	_style = style;
	_parent->LayerUpdated(this, region_infinite);
}

void Layer::SetUnredirected() {
	if (IsEmpty()) { return; }
	if (_is_redirected) {
		_is_redirected = false;
		_tile_cache.Init(AcquireResourceManager(),
						 CalculateTileSize(_composite_region.size, _style._width_change, _style._height_change),
						 _parent->GetStyle().background,          // Must be a base layer.
						 { point_zero, _composite_region.size },  //
						 _visible_region,
						 2.0);

		_is_redrawing_child = true;
		RefreshChildVisibleRegion();
		_is_redrawing_child = false;
	}
}

void Layer::RefreshVisibleRegion(Rect parent_visible_region) {
	// The parent_visible_region is region_empty by default, 
	//   which means the region is not specified, need to query first.
	if (parent_visible_region == region_empty) { 
		parent_visible_region = _parent->GetVisibleRegion(); 
	}

	_visible_region = parent_visible_region.Intersect(_composite_region) - (_composite_region.point - _current_point);

	// If the new visible region falls out of the cached region, reset the cached region, and redraw child windows.
	Rect old_cached_region = _tile_cache.GetCachedRegion();
	if (!old_cached_region.Contains(_visible_region)) {
		_tile_cache.ResetCachedRegion(_visible_region);
		Rect new_cached_region = _tile_cache.GetCachedRegion();

		// Set the tile mask to be the intersection of old and new cached regions, so these old tiles
		//   will not be redrawed when child windows redraw only on new tiles.
		_tile_cache.SetTileMask(old_cached_region.Intersect(new_cached_region), 
								TileCache::TileMaskAttribute::AccessDenied);

		// Refresh the child window's visible region, and redraw the child windows. (virtual function call)
		_is_redrawing_child = true;
		RefreshChildVisibleRegion();
		_is_redrawing_child = false;

		// Clear the tile mask.
		_tile_cache.ClearTileMask();
	}

	// Inform the parent window to recomposite.
	_parent->LayerUpdated(this, _tile_cache.GetCachedRegion());
}

TargetResourceManager* Layer::AcquireResourceManager() {
	return _parent->GetCanvas().GetLayer()->_tile_cache.GetResourceManager();
}

Rect Layer::DrawFigureQueue(FigureQueue& figure_queue, Vector position_offset, Rect bounding_region,
							bool commit_immediately) {
	Rect updated_region;

	if (_is_redirected) {
		// The layer is redirected to the parent's canvas, so it must be a base layer.
		// The positioning of the layer is the same as the parent's canvas, so there is no need to do conversion.
		// The final layer that eventually do the drawing will not commit immediately to 
		//   its parent after drawing. Because there will be recomposite happening to its child windows.
		updated_region = _parent->GetCanvas().DrawFigureQueue(figure_queue, position_offset, bounding_region, false);
	} else {
		updated_region = _tile_cache.DrawFigureQueue(figure_queue, position_offset, bounding_region);
	}

	if (!_is_redrawing_child && commit_immediately) {
		// Notify the parent window to composite.
		_parent->LayerUpdated(this, updated_region);
	}

	return updated_region;
}



SingleWndLayer::SingleWndLayer() : Layer(), _wnd(nullptr) {}

SingleWndLayer::~SingleWndLayer() {
	Clear();
}

void SingleWndLayer::Init(Ref _WndBase_Impl* parent, Rect composite_region, Ref _WndBase_Impl* wnd,
						  WndStyle::RenderStyle style) {
	if (!IsEmpty()) { Clear(); }

	Layer::Init(parent, composite_region, false,
				{ point_zero, composite_region.size }, point_zero, ColorSet::White, // No background.
				style);
	_wnd = wnd;
	_wnd->SetCanvas(Canvas(this, { point_zero, composite_region.size }));
}

void SingleWndLayer::Clear() {
	if (_wnd != nullptr) {
		_wnd->SetCanvas(Canvas());
		_wnd = nullptr;
	}
	Layer::Clear();
}

void SingleWndLayer::RefreshChildVisibleRegion() const {
	_wnd->RefreshVisibleRegion();
}

void SingleWndLayer::SetCompositeRegion(const Rect& composite_region) {
	bool size_changed = (_composite_region.size != composite_region.size);
	_composite_region = composite_region;
	if (size_changed) {
		// The size has changed, reset the canvas, and redraw the child window.
		SetAccessibleRegion({ point_zero, _composite_region.size });
		_wnd->SetCanvas(Canvas(this, { point_zero, composite_region.size }));
	} else {
		// The size has not changed, just refresh the visible region for tile cache.
		RefreshVisibleRegion();
	}
}


void MultipleWndLayer::Init(Ref _WndBase_Impl* parent, Size size, bool is_redirected) {
	if (!IsEmpty()) { Clear(); }

	Layer::Init(parent, { point_zero, size }, is_redirected,
				{ point_zero, size }, point_zero, parent->GetStyle().background);
}

void MultipleWndLayer::Init(Ref _WndBase_Impl* parent, Rect composite_region,
							Rect accessible_region, Point current_point) {
	if (!IsEmpty()) { Clear(); }

	Layer::Init(parent, composite_region, false,
				accessible_region, current_point, _parent->GetStyle().background);
}

MultipleWndLayer::~MultipleWndLayer() { 
	Clear(); 
}

void MultipleWndLayer::AddWnd(Ref _WndBase_Impl* wnd, Rect region) {
	_wnds.Insert(wnd, region);
	wnd->SetCanvas(Canvas(this, region));  // The child window will redraw later by itself.
}

void MultipleWndLayer::RemoveWnd(Ref _WndBase_Impl* wnd) {
	Rect old_region = _wnds.Remove(wnd);
	wnd->SetCanvas(Canvas());

	_tile_cache.SetTileMask(old_region, TileCache::TileMaskAttribute::AccessOnly);
	_is_redrawing_child = true;

	// Redraw the background.
	if (_is_redirected) {
		FigureQueue figure_queue;
		figure_queue.Push(new Color(_parent->GetStyle().background), old_region);
		_parent->GetCanvas().DrawFigureQueue(figure_queue, vector_zero, old_region, false);
	} else {
		_tile_cache.DrawBackground(old_region);
	}

	// Redraw the overlapped windows.
	for (auto& it : _wnds.QueryIntersect(old_region)) {
		it->Composite();
	}

	_is_redrawing_child = false;
	_tile_cache.ClearTileMask();

	// Inform the parent window to recomposite.
	_parent->LayerUpdated(this, old_region);
}

void MultipleWndLayer::MoveWnd(Ref _WndBase_Impl* wnd, Rect new_region) {
	Rect old_region = _wnds.Move(wnd, new_region);
	Rect union_region = old_region.Union(new_region);

	_tile_cache.SetTileMask(union_region, TileCache::TileMaskAttribute::AccessOnly);
	_is_redrawing_child = true;

	// Redraw the background.
	if (_is_redirected) {
		FigureQueue figure_queue;
		figure_queue.Push(new Color(_parent->GetStyle().background), union_region);
		_parent->GetCanvas().DrawFigureQueue(figure_queue, vector_zero, union_region, false);
	} else {
		_tile_cache.DrawBackground(union_region);
	}

	// Redraw the overlapped windows.
	for (auto& it : _wnds.QueryIntersect(union_region)) {
		if (it == wnd) {
			it->SetCanvas(Canvas(this, new_region));
		} else {
			it->Composite();
		}
	}

	_is_redrawing_child = false;
	_tile_cache.ClearTileMask();

	// Inform the parent window to recomposite.
	_parent->LayerUpdated(this, union_region);
}

void MultipleWndLayer::RefreshChildVisibleRegion() const {
	Rect region_to_update = GetVisibleRegion();

	// Redraw the background.
	if (_is_redirected) {
		FigureQueue figure_queue;
		figure_queue.Push(new Color(_parent->GetStyle().background), region_to_update);
		_parent->GetCanvas().DrawFigureQueue(figure_queue, vector_zero, region_to_update, false);
	} else {
		const_cast<TileCache&>(_tile_cache).DrawBackground(region_to_update);
	}

	for (auto& it : _wnds.QueryIntersect(region_to_update)) {
		it->RefreshVisibleRegion();
	}
}


END_NAMESPACE(WndDesign)
