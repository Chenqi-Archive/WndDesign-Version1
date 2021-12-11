#include "table_layer.h"
#include "../window/table_impl.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


TableLayer::TableLayer(_Table_Impl& parent) :
	Layer(parent, region_empty, true, 
		  region_empty, point_zero, parent._WndBase_Impl::GetStyle().background) {
}

_Table_Impl& TableLayer::GetParent() const { return static_cast<_Table_Impl&>(_parent); }


bool TableLayer::SetWidth(uint new_width) {
	Size old_size = GetAccessibleRegion().size;
	if (old_size.width == new_width) { return false; }
	_tile_cache.SetAccessibleRegion(Rect(point_zero, Size(new_width, old_size.height)), GetLayerVisibleRegion());
	_current_point = BoundPointInRegion(_current_point, ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
	return true;
}

bool TableLayer::SetHeight(uint new_height) {
	Size old_size = GetAccessibleRegion().size;
	if (old_size.height == new_height) { return false; }
	_tile_cache.SetAccessibleRegion(Rect(point_zero, Size(old_size.width, new_height)), GetLayerVisibleRegion());
	_current_point = BoundPointInRegion(_current_point, ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
	return true;
}

bool TableLayer::SetCurrentPoint(Point point) {
	point = BoundPointInRegion(point, ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
	if (point == GetCurrentPoint()) { return false; }
	_current_point = point;
	RefreshCachedRegion(GetLayerVisibleRegion());
	return true;
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
	Rect region_without_border = ShrinkRegionByLength(GetAccessibleRegion(), GetParent().GetStyle().border._width);
	TableHitTestInfo first_grid_info = GetParent().HitTestGrid(
		BoundPointInRegion(region_to_update.point, region_without_border), true
	);
	TablePosition first_grid = first_grid_info.grid;
	Point end_point = region_to_update.RightBottom();  // The value must be greater than 0.

	// From the next grid, draw vertical grid lines. (Use solid rectangle as axis-aligned lines.)
	Background line_background(GetParent().GetTableStyle().grid_line._color);
	uint line_width = GetParent().GetTableStyle().grid_line._width;
	for (uint index_column = first_grid.column + 1; index_column < GetParent()._columns.size(); ++index_column) {
		if (GetParent()._columns[index_column].x - line_width >= static_cast<uint>(end_point.x)) { break; }
		figure_queue.Push(
			Rectangle::Create(0, color_transparent, line_background),
			Rect(GetParent()._columns[index_column].x - line_width, region_to_update.point.y, line_width, region_to_update.size.height)
		);
	}
	// Draw horizontal grid lines.
	for (uint index_row = first_grid.row + 1; index_row < GetParent()._rows.size(); ++index_row) {
		if (GetParent()._rows[index_row].y - line_width >= static_cast<uint>(end_point.y)) { break; }
		figure_queue.Push(
			Rectangle::Create(0, color_transparent, line_background),
			Rect(region_to_update.point.x, GetParent()._rows[index_row].y - line_width, region_to_update.size.width, line_width)
		);
	}

	if (IsRedirected()) {
		GetParent().RedirectLayerFigureQueue(figure_queue, region_to_update);  // No need to change coordinates.
	} else {
		const_cast<TileCache&>(_tile_cache).DrawFigureQueue(figure_queue, region_to_update, vector_zero);
	}

	// Draw related child windows.
	for (uint index_column = first_grid.column; index_column < GetParent()._columns.size(); ++index_column) {
		if (GetParent()._columns[index_column].x >= static_cast<uint>(end_point.x)) { break; }
		for (uint index_row = first_grid.row; index_row < GetParent()._rows.size(); ++index_row) {
			if (GetParent()._rows[index_row].y >= static_cast<uint>(end_point.y)) { break; }
			auto& child = GetParent()._child_wnds[index_column][index_row];
			if (child == nullptr) { continue; }
			Rect child_region_on_layer = child->RegionOnLayer();
			Rect child_region_to_composite = child_region_on_layer.Intersect(region_to_update) - (child_region_on_layer.point - point_zero);
			child->Composite(child_region_to_composite);
		}
	}
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