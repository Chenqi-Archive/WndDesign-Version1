#include "table_impl.h"
#include "../render/layer_figure.h"
#include "../style/table_style_helper.h"
#include "../geometry/geometry_helper.h"

#include "../common/stl_iterator_helper.h"
#include "../common/stl_vector_helper.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<ITable> ITable::Create(Ref<IWndBase*> parent, const TableStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<ITable> table = std::make_unique<_Table_Impl>(new TableStyle(style), object);
	if (parent != nullptr) { parent->AddChild(table.get()); }
	return table;
}



_Table_Impl::_Table_Impl(Alloc<TableStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object), _layer(*this) {
	CheckGridSize(*style);
}


uint _Table_Impl::RecalculateColumnPosition(vector<Column>::iterator it_column) {
	uint border_width = GetTableStyle().border._width;
	uint line_width = GetTableStyle().grid_line._width;

	uint index_column_begin = static_cast<uint>(it_column - _columns.begin());
	uint column_position_begin = index_column_begin == 0 ?
		border_width : _columns[index_column_begin - 1].x + _columns[index_column_begin - 1].width + line_width;

	for (uint index_column = index_column_begin; index_column < _columns.size(); ++index_column) {
		_columns[index_column].x = column_position_begin;
		column_position_begin += _columns[index_column].width + line_width;
	}

	// If there's no column, the line width should be removed.
	uint layer_width =  _columns.empty() ? 
		column_position_begin + border_width : column_position_begin + border_width - line_width;
	_layer.SetWidth(layer_width);
	return layer_width;
}

uint _Table_Impl::RecalculateRowPosition(vector<Row>::iterator it_row) {
	uint border_width = GetTableStyle().border._width;
	uint line_width = GetTableStyle().grid_line._width;

	uint index_row_begin = static_cast<uint>(it_row - _rows.begin());
	uint row_position_begin = index_row_begin == 0 ?
		border_width : _rows[index_row_begin - 1].y + _rows[index_row_begin - 1].height + line_width;

	for (uint index_row = index_row_begin; index_row < _rows.size(); ++index_row) {
		_rows[index_row].y = row_position_begin;
		row_position_begin += _rows[index_row].height + line_width;
	}

	uint layer_height = _rows.empty() ?
		row_position_begin + border_width : row_position_begin + border_width - line_width;
	_layer.SetHeight(layer_height);
	return layer_height;
}

vector<_Table_Impl::Column>::iterator _Table_Impl::RecalculateColumnWidth(uint window_width) {
	auto it_column_width_changed = _columns.end();
	if (GetStyle().auto_resize._width_auto_resize || window_width == GetSize().width) { 
		return it_column_width_changed;  // No need to resize or the height won't change.
	}

	for (auto it = _columns.rbegin(); it != _columns.rend(); ++it) {
		if (it->width_tag.IsAuto()) { continue; } // The column width is dependent on child windows.
		uint new_width = CalculateColumnWidth(GetTableStyle(), window_width, it->width_tag);
		if (new_width != it->width) {
			it->width = new_width;
			it_column_width_changed = get_cooresponding_iterator(it);
		}
	}
	return it_column_width_changed; // Return the current column for resetting canvas for child windows.
}

vector<_Table_Impl::Row>::iterator _Table_Impl::RecalculateRowHeight(uint window_height) {  // Similar to above.
	auto it_row_height_changed = _rows.end();

	if (GetStyle().auto_resize._height_auto_resize || window_height == GetSize().height) {
		return it_row_height_changed;
	}

	for (auto it = _rows.rbegin(); it != _rows.rend(); ++it) {
		if (it->height_tag.IsAuto()) { continue; }
		uint new_height = CalculateRowHeight(GetTableStyle(), window_height, it->height_tag);
		if (new_height != it->height) {
			it->height = new_height;
			it_row_height_changed = get_cooresponding_iterator(it);
		}
	}
	return it_row_height_changed; // Return the current row for resetting canvas for child windows.
}

const TableHitTestInfo _Table_Impl::HitTestGridOnLayer(Point point) const {
	uint border_width = GetTableStyle().border._width;
	Size size = GetLayerSize();  // Layer size is the grid size plus border width.
	uint x = static_cast<uint>(point.x), y = static_cast<uint>(point.y);

	if(x >= size.width || y >= size.height){ return { TableHitTestInfo::Result::OutSide , table_position_invalid , point };}
	if(x < border_width) { return { TableHitTestInfo::Result::BorderLeft , table_position_invalid , point }; }
	if(y < border_width) { return { TableHitTestInfo::Result::BorderTop , table_position_invalid , point };}
	if(x >= size.width - border_width) {return { TableHitTestInfo::Result::BorderRight , table_position_invalid , point };}
	if(y >= size.height - border_width) { return { TableHitTestInfo::Result::BorderDown , table_position_invalid , point }; }

	TableHitTestInfo::Result type = TableHitTestInfo::Result::Grid;

	// Find the last element that is less than or equal to the value.
	// Use reverse iterator, if no such element, return rend().
	auto cmp_row = [](const Row& row, uint y) { return row.y > y ? true : false; };
	auto it_row = std::lower_bound(_rows.rbegin(), _rows.rend(), y, cmp_row);
	if (it_row == _rows.rend()) { Error(); }				// Hit on the top border.
	if (y - it_row->y >= it_row->height) {
		if (it_row == _rows.rbegin()) { Error(); }			// Hit on the bottom border.
		type = TableHitTestInfo::Result::GridLineBottom;	// Hit on the bottom grid line.
	}
	auto cmp_column = [](const Column& column, uint x) { return column.x > x ? true : false; };
	auto it_column = std::lower_bound(_columns.rbegin(), _columns.rend(), x, cmp_column);
	if (it_column == _columns.rend()) { Error(); }				// Hit on the left border.
	if (x - it_column->x >= it_column->width) { 
		if (it_column == _columns.rbegin()) { Error(); }	// Hit on the right border.
		type = TableHitTestInfo::Result::GridLineRight;	// Hit on the right grid line.
	} 
	return TableHitTestInfo{
		type,
		TablePosition(
			static_cast<uint>(get_cooresponding_iterator(it_column) - _columns.begin()), 
			static_cast<uint>(get_cooresponding_iterator(it_row) - _rows.begin())
		),
		point - (Point(it_column->x, it_row->y) - point_zero)
	};
}

const TableHitTestInfo _Table_Impl::HitTestGrid(Point point, bool point_on_layer) const {
	if (!point_on_layer) {  // Point is relative to the window.
		// First check if the point falls in the window.
		uint border_width = GetStyle().border._width;
		Size size = GetSize();
		uint x = static_cast<uint>(point.x), y = static_cast<uint>(point.y);  // negative int --> large uint.
		if (x >= size.width || y >= size.height) { return { TableHitTestInfo::Result::OutSide , table_position_invalid , point }; }
		if (x < border_width) { return { TableHitTestInfo::Result::BorderLeft , table_position_invalid , point }; }
		if (y < border_width) { return { TableHitTestInfo::Result::BorderTop , table_position_invalid , point }; }
		if (x >= size.width - border_width) { return { TableHitTestInfo::Result::BorderRight , table_position_invalid , point }; }
		if (y >= size.height - border_width) { return { TableHitTestInfo::Result::BorderDown , table_position_invalid , point }; }
		// Then convert point to point on layer.
		point = _layer.ConvertToLayerPoint(point);
	}
	return HitTestGridOnLayer(point);
}


void _Table_Impl::SetTableSize(TableSize size, ValueTag width, ValueTag height) {
	// Resize columns and rows.
	uint column_width = CalculateColumnWidth(GetTableStyle(), GetSize().width, width);
	if (width.IsAuto() && GetTableStyle().grid_size._width_fixed) { width.Set(column_width); }
	auto it_column = resize_with_element(_columns, size.column_number, Column(width, column_width));

	uint row_height = CalculateRowHeight(GetTableStyle(), GetSize().height, height);
	if (height.IsAuto() && GetTableStyle().grid_size._height_fixed) { height.Set(row_height); }
	auto it_row = resize_with_element(_rows, size.row_number, Row(height, row_height));

	// Resize child windows. Nullptr will be added, and trimmed child windows will destruct.
	_child_wnds.resize(size.column_number);
	for (auto& it : _child_wnds) { it.resize(size.row_number); }

	// Recalculate row and column position. But no need to reset child position.
	uint layer_width = RecalculateColumnPosition(it_column);
	uint layer_height = RecalculateRowPosition(it_row);

	// Resize the base layer and auto fit window size.
	Size unchanged_region(it_column == _columns.end() ? layer_width : it_column->x, it_row == _rows.end() ? layer_height : it_row->y);
	LayerSizeUpdated(unchanged_region);
}

void _Table_Impl::InsertColumn(uint column_begin, uint column_cnt, ValueTag width) {
	uint column_width = CalculateColumnWidth(GetTableStyle(), GetSize().width, width);
	if (width.IsAuto() && GetTableStyle().grid_size._width_fixed) { width.Set(column_width); }
	auto it_column = insert_from_begin_by_cnt(_columns, column_begin, column_cnt, Column(width, column_width));
	if (it_column == _columns.end()) { return; }  // No column has been inserted.

	insert_from_begin_by_cnt(_child_wnds, column_begin, column_cnt, vector<ChildContainer>(_rows.size()));

	uint layer_width = RecalculateColumnPosition(it_column);
	ResetChildPosition(it_column + column_cnt);

	// Resize the base layer and auto fit window size.
	LayerSizeUpdated(Size(it_column->x, GetLayerSize().height));
}

void _Table_Impl::InsertRow(uint row_begin, uint row_cnt, ValueTag height) {
	uint row_height = CalculateRowHeight(GetTableStyle(), GetSize().height, height);
	if (height.IsAuto() && GetTableStyle().grid_size._height_fixed) { height.Set(row_height); }
	auto it_row = insert_from_begin_by_cnt(_rows, row_begin, row_cnt, Row(height, row_height));
	if (it_row == _rows.end()) { return; }

	for (auto& it : _child_wnds) { insert_from_begin_by_cnt(it, row_begin, row_cnt, ChildContainer()); }

	uint layer_height = RecalculateRowPosition(it_row);
	ResetChildPosition(it_row + row_cnt);

	// Resize the base layer and auto fit window size.
	LayerSizeUpdated(Size(GetLayerSize().width, it_row->y));
}

void _Table_Impl::DeleteColumn(uint column_begin, uint column_cnt) {
	auto it_column = erase_from_begin_by_cnt(_columns, column_begin, column_cnt);

	erase_from_begin_by_cnt(_child_wnds, column_begin, column_cnt);

	uint layer_width = RecalculateColumnPosition(it_column);
	ResetChildPosition(it_column);

	// Resize the base layer and auto fit window size.
	LayerSizeUpdated(Size(it_column == _columns.end() ? layer_width : it_column->x, GetLayerSize().height));
}

void _Table_Impl::DeleteRow(uint row_begin, uint row_cnt) {
	auto it_row = erase_from_begin_by_cnt(_rows, row_begin, row_cnt);

	for (auto& it : _child_wnds) { erase_from_begin_by_cnt(it, row_begin, row_cnt); }

	uint layer_height = RecalculateRowPosition(it_row);
	ResetChildPosition(it_row);

	// Resize the base layer and auto fit window size.
	LayerSizeUpdated(Size(GetLayerSize().width, it_row == _rows.end() ? layer_height : it_row->y));
}

void _Table_Impl::SetColumnWidth(uint column, uint column_cnt, ValueTag width) {
	auto it_column = get_iterator_by_index(_columns, column);
	if (it_column == _columns.end()) { return; }

	uint column_width = CalculateColumnWidth(GetTableStyle(), GetSize().width, width);
	if (width.IsAuto()) { width.Set(column_width); }

	for (auto it = it_column; it != _columns.end() && column_cnt != 0; ++it, --column_cnt) {
		it->width_tag = width;
		it->width = column_width;
	}

	uint layer_width = RecalculateColumnPosition(it_column + 1);

	auto it_row = AdjustRowHeightToColumnWidthChange();
	uint layer_height = RecalculateRowPosition(it_row);
	ResetChildPosition(it_column, it_row);

	LayerSizeUpdated(Size(it_column->x, it_row == _rows.end() ? layer_height : it_row->y));
}

void _Table_Impl::SetRowHeight(uint row, uint row_cnt, ValueTag height) {
	auto it_row = get_iterator_by_index(_rows, row);
	if (it_row == _rows.end()) { return; }

	uint row_height = CalculateRowHeight(GetTableStyle(), GetSize().height, height);
	if (height.IsAuto()) { height.Set(row_height); }

	for (auto it = it_row; it != _rows.end() && row_cnt != 0; ++it, --row_cnt) {
		it->height_tag = height;
		it->height = row_height;
	}

	uint layer_height = RecalculateRowPosition(it_row + 1);

	auto it_column = AdjustColumnWidthToRowHeightChange();
	uint layer_width = RecalculateColumnPosition(it_column);
	ResetChildPosition(it_column, it_row);

	LayerSizeUpdated(Size(it_column == _columns.end() ? layer_width : it_column->x, it_row->y));
}


vector<_Table_Impl::Row>::iterator _Table_Impl::AdjustRowHeightToColumnWidthChange() {
	auto it_first_row_height_changed = _rows.end();
	for (int index_row = static_cast<int>(_rows.size()) - 1; index_row >= 0; --index_row) {
		if (_rows[index_row].HeightFixed()) { continue; }
		uint required_height = 0;
		for (uint index_column = 0; index_column < _columns.size(); ++index_column) {
			auto& current_child = _child_wnds[index_column][index_row];
			if (current_child == nullptr) { continue; }
			uint current_height = current_child->CalculateRegionOnParent(
				Size(_columns[index_column].width, _rows[index_row].height)
			).size.height;
			if (required_height < current_height) { required_height = current_height; }
		}
		// Bound required height between max and min height.
		required_height = CalculateRowHeight(GetTableStyle(), GetSize().height, ValueTag(required_height));
		if (required_height != _rows[index_row].height) {
			_rows[index_row].height = required_height;
			it_first_row_height_changed = _rows.begin() + index_row;
		}
	}
	return it_first_row_height_changed;
}

vector<_Table_Impl::Column>::iterator _Table_Impl::AdjustColumnWidthToRowHeightChange() {
	auto it_first_column_width_changed = _columns.end();
	for (int index_column = static_cast<int>(_columns.size()) - 1; index_column >= 0; --index_column) {
		if (_columns[index_column].WidthFixed()) { continue; }
		uint required_width = 0;
		for (uint index_row = 0; index_row < _rows.size(); ++index_row) {
			auto& current_child = _child_wnds[index_column][index_row];
			if (current_child == nullptr) { continue; }
			uint current_width = current_child->CalculateRegionOnParent(
				Size(_columns[index_column].width, _rows[index_row].height)
			).size.width;
			if (required_width < current_width) { required_width = current_width; }
		}
		required_width = CalculateColumnWidth(GetTableStyle(), GetSize().height, ValueTag(required_width));
		if (required_width != _columns[index_column].width) {
			_columns[index_column].width = required_width;
			it_first_column_width_changed = _columns.begin() + index_column;
		}
	}
	return it_first_column_width_changed;
}


void _Table_Impl::ResetChildPosition(vector<Column>::iterator it_column) {
	for (uint index_column = static_cast<uint>(it_column - _columns.begin()); index_column < _columns.size(); ++index_column) {
		for (uint index_row = 0; index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			SetChildCanvas(
				*_child_wnds[index_column][index_row],
				Rect(_columns[index_column].x, _rows[index_row].y, _columns[index_column].width, _rows[index_row].height)
			);
		}
	}
}

void _Table_Impl::ResetChildPosition(vector<Row>::iterator it_row) {
	for (uint index_column = 0; index_column < _columns.size(); ++index_column) {
		for (uint index_row = static_cast<uint>(it_row - _rows.begin()); index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			SetChildCanvas(
				*_child_wnds[index_column][index_row],
				Rect(_columns[index_column].x, _rows[index_row].y, _columns[index_column].width, _rows[index_row].height)
			);
		}
	}
}

void _Table_Impl::ResetChildPosition(vector<Column>::iterator it_column, vector<Row>::iterator it_row) {
	ResetChildPosition(it_column);
	// Similar to ResetChildPosition(it_row), but only changes the windows before it_column.
	for (uint index_column = 0; index_column < it_column - _columns.begin(); ++index_column) {
		for (uint index_row = static_cast<uint>(it_row - _rows.begin()); index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			SetChildCanvas(
				*_child_wnds[index_column][index_row],
				Rect(_columns[index_column].x, _rows[index_row].y, _columns[index_column].width, _rows[index_row].height)
			);
		}
	}
}


TablePosition _Table_Impl::GetChildPosition(_WndBase_Impl& child) {
	// Get the position by referencing child's region on layer.
	TableHitTestInfo info = HitTestGrid(child.RegionOnLayer().point, true);
	if (info.result != TableHitTestInfo::Result::Grid) { Error(); }
	if (_child_wnds[info.grid.column][info.grid.row] != &child) { Error(); } // Double check if the window is really the child.
	return info.grid;
}

bool _Table_Impl::UpdateChildRegion(Ref<_WndBase_Impl*> child, TablePosition grid) {
	// If both row height and column width is fixed, return unchanged.
	if (_rows[grid.row].HeightFixed() && _columns[grid.column].WidthFixed()) { return false; }

	// Calculate the new size of child window, the point will be ignored. 
	// The current grid size is the parent size.
	const Size grid_size(_columns[grid.column].width, _rows[grid.row].height); // An alias for current grid size.
	Size required_size = size_min;
	if (child != nullptr) {   // If child == nullptr, the required_size is size_zero.
		required_size = child->CalculateRegionOnParent(grid_size).size; 
	}

	// Bound the size based on whether the size is fixed.
	if (_rows[grid.row].HeightFixed()) {
		required_size.height = grid_size.height; 
	} else if (_columns[grid.column].WidthFixed()) { // Use "else" because if height is fixed, then width must not be fixed.
		required_size.width = grid_size.width; 
	}
	// Bound the size between min and max grid size.
	required_size = BoundGridSize(GetTableStyle(), GetSize(), required_size);

	if (required_size == grid_size) { return false; } // Return if size is not changed.


	// If the required size is smaller than the current size, the column/row may shrink to fit the size, 
	//   but before that, all child windows' width of the same column/row will be calculated to make sure no 
	//   child window requires a larger size.
	// If the required size is larger than the current size, simply use the required size, because if there's another 
	//   child window of the same column/row who requires a larger size, the column/row's size must have already fitted to it.
	if (required_size.width < grid_size.width) {
		// If there are child windows of the same column who requires a larger width, increase the reqired width.
		for (uint index_row = 0; index_row < _rows.size(); ++index_row) {
			auto& current_child = _child_wnds[grid.column][index_row];
			if (current_child == child || current_child == nullptr) { continue; }
			// Calculate the width based on grid size. Point and height is ignored. (In fact there's no need to provide row height.)
			uint current_width = current_child->CalculateRegionOnParent(Size(grid_size.width, _rows[index_row].height)).size.width;
			if (required_size.width < current_width) { required_size.width = current_width; }
			if (required_size.width >= grid_size.width) { break; }
		}
		// required_size will never be larger than the grid size.
		// This exception may occur when window size style is relative(in percent) while the grid size style is auto.
		if (required_size.width > grid_size.width) { ExceptionDialog(); }
	}
	if (required_size.height < grid_size.height) { // Similar to above.
		for (uint index_column = 0; index_column < _columns.size(); ++index_column) {
			auto& current_child = _child_wnds[index_column][grid.row];
			if (current_child == child || current_child == nullptr) { continue; }
			uint current_height = current_child->CalculateRegionOnParent(Size(_columns[index_column].width, grid_size.height)).size.height;
			if (required_size.height < current_height) { required_size.height = current_height; }
			if (required_size.height >= grid_size.height) { break; }
		}
		if (required_size.height > grid_size.height) { ExceptionDialog(); }
	}

	if (required_size == grid_size) { return false; } // Return if size is still not changed.

	// Change the size of current column/row, reset the position for subsequent column/rows.
	auto it_row = _rows.end(); auto it_column = _columns.end();
	Size unchanged_region = GetLayerSize();
	if (grid_size.width != required_size.width) {
		it_column = _columns.begin() + grid.column;  // Point to current column.
		it_column->width = required_size.width;		 // Set the width of current column.
		RecalculateColumnPosition(it_column + 1);  // Calculate the position from the next column to the end.
		unchanged_region.width = it_column->x;
	}
	if (grid_size.height != required_size.height) {
		it_row = _rows.begin() + grid.row;
		it_row->height = required_size.height;
		RecalculateRowPosition(it_row + 1);
		unchanged_region.height = it_row->y;
	}

	ResetChildPosition(it_column, it_row); // Reset canvas for subsequent child windows.

	LayerSizeUpdated(unchanged_region);
	return true;
}

bool _Table_Impl::SetChild(Ref<IWndBase*> child_wnd, TablePosition position) {
	if (position.column >= _columns.size() || position.row >= _rows.size()) { return false; }
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	_child_wnds[position.column][position.row] = std::move(ChildContainer(*this, child));
	bool region_updated = UpdateChildRegion(child, position);
	if (region_updated) { return true; }  // The region has changed, and child has got canvas.

	// The region is fixed or not changed, set canvas for the child.
	if (child != nullptr) {
		SetChildCanvas(
			*child,
			Rect(_columns[position.column].x, _rows[position.row].y, _columns[position.column].width, _rows[position.row].height)
		);
	}
	return true;
}


void _Table_Impl::LayerSizeUpdated(Size unchanged_region) {
	Size layer_size = GetLayerSize();
	// Redraw the new region on layer if layer is redirected.
	if (!_layer.IsRedirected()) {
		if (unchanged_region.width < layer_size.width) {
			_layer.RegionUpdated(
				Rect(static_cast<int>(unchanged_region.width), 0, layer_size.width - unchanged_region.width, unchanged_region.height), 
				false
			);
		}
		if (unchanged_region.height < layer_size.height) {
			_layer.RegionUpdated(
				Rect(0, static_cast<int>(unchanged_region.height), layer_size.width, layer_size.height - unchanged_region.height),
				false
			);
		}
	}

	// The layer's size has changed due to the insertion or deletion of rows/columns or the resize of child windows,
	// If window will auto resize, inform parent to recalculate the region, the size style will be reset when
	//   CalculateRegionOnParent() is called.
	bool size_changed = ContentUpdated();
	if (size_changed) { return; }  // The window has been redrawn by parent, no need to update the region again.

	// The parent window does not auto resize, update the changed_region.
	// (If the unchanged_region relative to parent entirely overlaps the parent size, don't update.
	Size size = GetSize();
	if (unchanged_region.width < size.width) {
		UpdateRegion(Rect(static_cast<int>(unchanged_region.width), 0, size.width - unchanged_region.width, unchanged_region.height));
	}
	if (unchanged_region.height < size.height) {
		UpdateRegion(Rect(0, static_cast<int>(unchanged_region.height), size.width, size.height - unchanged_region.height));
	}
}


Size _Table_Impl::AdjustSizeToContent(Size min_size, Size max_size) {
	// Use the layer size as the new normal size.
	// The size will be bounded between min and max size when calculating the size.
	return GetLayerSize();
}

uint _Table_Impl::AdjustHeightToContent(uint min_height, uint max_height, uint width) {
	uint layer_height = GetLayerSize().height;

	// The width is fixed but height may auto change.
	// Use window_width to calculate column width and recalculate all child windows' height.
	auto it_column = RecalculateColumnWidth(width);
	if (it_column == _columns.end()) { return layer_height; }  // The column width didn't change, return the layer height.
	RecalculateColumnPosition(it_column + 1); // Reset position from the next column.

	auto it_row = AdjustRowHeightToColumnWidthChange();
	if (it_row != _rows.end()) { layer_height = RecalculateRowPosition(it_row + 1); }
	ResetChildPosition(it_column, it_row);
	return layer_height;
}

uint _Table_Impl::AdjustWidthToContent(uint min_width, uint max_width, uint height) {
	uint window_width = GetLayerSize().width;

	// The height is fixed but width may auto change.
	// Use height to calculate column width and recalculate all child windows' width.
	auto it_row = RecalculateRowHeight(height);
	if (it_row == _rows.end()) { return window_width; }  // The column width didn't change, return the layer width.
	RecalculateRowPosition(it_row + 1); // Reset position from the next row.

	auto it_column = AdjustColumnWidthToRowHeightChange();
	if (it_column != _columns.end()) { window_width = RecalculateColumnPosition(it_column + 1); }
	ResetChildPosition(it_column, it_row);  // May set when size has really changed.
	return window_width;
}


void _Table_Impl::SizeChanged() {
	Size size = GetSize();
	_layer.ParentSizeChanged(size);

	// If both auto resize, the row/column size and the layer size don't need to change.
	// If width not auto resize but height auto resize, the width has been calculated and height has been adjusted, and vice versa.
	//   (Hopefully parent will always provide the same region as requried, so assume the width or height won't change again.)
	// If both not auto resize, recalculate the column/row size.
	if (GetStyle().auto_resize._NotAutoResize()) {
		// Resize accroding to the window size.
		auto it_column_resized = RecalculateColumnWidth(size.width);
		auto it_row_resized = RecalculateRowHeight(size.height);

		// Adjust accroding to the other dimension.
		auto it_column_adjusted = _columns.end();
		auto it_row_adjusted = _rows.end();
		if (it_row_resized != _rows.end()) { it_column_adjusted = AdjustColumnWidthToRowHeightChange(); }
		if (it_column_resized != _columns.end()) { it_row_adjusted = AdjustRowHeightToColumnWidthChange(); }

		Size unchanged_region = GetLayerSize();
		auto it_column = std::min(it_column_resized, it_column_adjusted);
		auto it_row = std::min(it_row_resized, it_row_adjusted);
		if (it_column != _columns.end()) { RecalculateColumnPosition(it_column + 1); unchanged_region.width = it_column->x; }
		if (it_row != _rows.end()) { RecalculateRowPosition(it_row + 1); unchanged_region.height = it_row->y; }
		ResetChildPosition(it_column, it_row);

		LayerSizeUpdated(unchanged_region);
	}

	// If does not auto resize, recalculate the size of columns and rows.
	// Hopefully no row or column did actually resize, because parent will always provide the same region as requried.
//	auto style = GetStyle();
//	if (!style.auto_resize._width_auto_resize) { auto it_column = RecalculateColumnWidth(size.width); }
//	auto it_row = RecalculateRowHeight(size.height);
//	ResetChildPosition(it_column, it_row); // Reset canvas for subsequent child windows.

	// Set the layer to be redirected and scrollable if size is smaller than layer size.
	if (!_layer.IsRedirected() && GetStyle().auto_resize._enable_scrolling == true) {
		Size layer_size = GetLayerSize();
		if (size.width < layer_size.width || size.height < layer_size.height) {
			_layer.SetUnredirected();  // The layer will be unredirected and the whole region will be updated.
		}
	}
}


void _Table_Impl::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const {
	if (_layer.IsRedirected()) {  // No need to change coordinates for redirected layers.
		_layer.CompositeChild(region_to_update);
	} else {
		// No need to calculate intersected region.
		figure_queue.append(new LayerFigure(_layer, _layer.ConvertToLayerPoint(region_to_update.point)), region_to_update);
	}

	// Draw the border as a rectangle. Rounded rectangle may be supported later, which requires clipping.
	auto& style = GetStyle();
	if (style.border._width > 0 && style.border._color != 0) {
		figure_queue.append(
			new Rectangle(style.border._width, style.border._color, background_transparent),
			Rect(point_zero, GetSize())
		);
	}

}


END_NAMESPACE(WndDesign)