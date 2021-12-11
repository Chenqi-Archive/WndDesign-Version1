#include "table_layer.h"
#include "../window/table_impl.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


TableLayer::TableLayer(_Table_Impl& parent) :
	Layer(parent, region_empty, true, 
		  region_empty, point_zero, parent._WndBase_Impl::GetStyle().background) {
}


void TableLayer::SetLayerSize(Size size) {
	if (GetAccessibleRegion().size == size) { return; }
	_composite_region.size = GetParent().GetSize();
	_tile_cache.SetAccessibleRegion(Rect(point_zero, size), GetLayerVisibleRegion());
	// The layer will be updated by Table.
}

void TableLayer::RefreshChildVisibleRegion(Rect cached_region) const {
	for (auto& it_vector : GetParent()._child_wnds) {
		for (auto& it_wnd : it_vector) {
			if (it_wnd == nullptr) { continue; }
			Rect child_region_on_layer = it_wnd->RegionOnLayer();
			Rect region_on_child = child_region_on_layer.Intersect(cached_region) - (child_region_on_layer.point - point_zero);
			it_wnd->RefreshVisibleRegion(region_on_child);
		}
	}
}

void TableLayer::RedrawChild(Rect region_to_update) const {
	// Redraw the background.
	FigureQueue figure_queue;
	if (IsRedirected()) {
		figure_queue.Push(BackgroundFigure::Create(_parent.GetStyle().background, region_to_update.point), region_to_update);
	} else {
		const_cast<TileCache&>(_tile_cache).DrawBackground(region_to_update);
	}

	// Draw grid lines. Border will be drawn by parent window when compositing.
	_Table_Impl::HitTestGridInfo first_grid_info = GetParent().HitTestGrid(region_to_update.point, true);
	if(first_grid_info.)
	TablePosition first_grid = first_grid_info.grid;


	if (IsRedirected()) {
		GetParent().RedirectLayerFigureQueue(figure_queue, region_to_update);  // No need to change coordinates.
	} else {
		const_cast<TileCache&>(_tile_cache).DrawFigureQueue(figure_queue, region_to_update, vector_zero);
	}

	// Draw related child windows.


}

void TableLayer::SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) {
	for (auto& it_vector : GetParent()._child_wnds) {
		for (auto& it_wnd : it_vector) {
			if (it_wnd == nullptr) { continue; }
			it_wnd->ResetLayerResourceManager(resource_manager);
		}
	}
}


END_NAMESPACE(WndDesign)