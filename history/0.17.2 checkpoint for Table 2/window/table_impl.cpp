#include "table_impl.h"
#include "../style/table_style_helper.h"
#include "../render/layer_figure.h"

#include <algorithm>


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWndBase, IWndBase::Deleter> 
ITable::Create(Ref<IWndBase*> parent, const TableStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<ITable, IWndBase::Deleter> table(new _Table_Impl(new TableStyle(style), object));
	table->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(table.get()); }
	return table;
}


// Helper functions.
// Get the vector iterator by index.
// If index is larger than the size, returns the end.
template <class T>
inline auto get_iterator_by_index(const std::vector<T>& vector, uint index) {
	if (index < vector.size()) { return vector.begin() + index; }
	return vector.end();
}
// Non-const version.
template <class T>
inline auto get_iterator_by_index(std::vector<T>& vector, uint index) {
	if (index < vector.size()) { return vector.begin() + index; }
	return vector.end();
}

// Erase cnt elements of the vector from begin.
// Returns the iterator pointing to the next unerased element.
// If cnt is zero, returns the end.
template<class T>
inline auto erase_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt) {
	auto it = get_iterator_by_index(vector, begin);
	if (it != vector.end() && cnt > 0) {
		auto it_end = get_iterator_by_index(vector, begin + cnt);
		it = vector.erase(it, it_end);
		return it;
	} else {
		return vector.end();
	}
}

// Insert cnt elements to the vector from begin.
// Returns the iterator pointing to the first inserted element.
// If no element was inserted, returns the end.
template<class T>
inline auto insert_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt, const T& element) {
	if (cnt == 0) { return vector.end(); }
	auto it_begin = get_iterator_by_index(vector, begin);
	return vector.insert(it_begin, cnt, element);
}

// Resize the vector to the desired size.
// If desired size is larger than the original size, insert the specific element.
// Returns the iterator pointing to the end or the first inserted element.
template<class T>
inline auto resize_with_element(std::vector<T>& vector, uint new_size, const T& element) {
	uint old_size = vector.size();
	vector.resize(new_size, element);
	return get_iterator_by_index(vector, old_size);
}



_Table_Impl::_Table_Impl(Alloc<TableStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object), _layer(*this) {
	CheckGridSize(*style);
}


bool _Table_Impl::SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) {
	TableStyle& style = const_cast<TableStyle&>(GetStyle());
	bool has_reset = false;
	if (!width.IsAuto()) { style.size._normal.width = width; style.auto_resize._width_auto_resize = false; has_reset = true; }
	if (!height.IsAuto()) { style.size._normal.height = height; style.auto_resize._height_auto_resize = false; has_reset = true; }
	if (!left.IsAuto()) { style.position._left = left; has_reset = true; }
	if (!top.IsAuto()) { style.position._top = top; has_reset = true; }
	if (!has_reset) { return false; }
	if (GetParent() != nullptr) { return GetParent()->UpdateChildRegion(this); }
	return false;
}

void _Table_Impl::UpdateLayerSize(Size layer_size) {
	// Reset the size of layer.


	// The layer's size has changed due to the insertion or deletion of rows/columns or the resize of child windows,
	// If window will auto resize, inform parent to recalculate the region, the size style will be reset when
	//   CalculateRegionOnParent() is called.
	if (GetStyle().auto_resize.WillAutoResize() && GetParent() != nullptr) {
		bool size_changed = GetParent()->UpdateChildRegion(this);
		if (size_changed) { return; }  // The window has been redrawn by parent.
	}

	// The parent window does not auto resize, update the whole region to redraw.
#pragma message("To do: In fact only the modified region need to be redrawn.")
	RegionUpdated(region_infinite);
}


uint _Table_Impl::RecalculateColumnPosition(vector<Column>::iterator it_column) {
	uint border_width = GetStyle().border._width;
	uint line_width = GetStyle().grid_line._width;
	uint column_position_begin;

	uint index_column_begin = it_column - _columns.begin();
	if (index_column_begin == 0) {
		column_position_begin = border_width;
	} else {
		column_position_begin = _columns[index_column_begin - 1].x + _columns[index_column_begin - 1].width + line_width;
	}

	for (uint index_column = index_column_begin; index_column < _columns.size(); ++index_column) {
		_columns[index_column].x = column_position_begin;
		column_position_begin += _columns[index_column].width + line_width;
	}

	if (_columns.empty()) {
		return column_position_begin + border_width;
	} else {
		return column_position_begin + border_width - line_width;
	}
}

uint _Table_Impl::RecalculateRowPosition(vector<Row>::iterator it_row) {
	uint border_width = GetStyle().border._width;
	uint line_width = GetStyle().grid_line._width;
	uint row_position_begin;

	uint index_row_begin = it_row - _rows.begin();
	if (index_row_begin == 0) {
		row_position_begin = border_width;
	} else {
		row_position_begin = _rows[index_row_begin - 1].y + _rows[index_row_begin - 1].height + line_width;
	}

	for (uint index_row = index_row_begin; index_row < _rows.size(); ++index_row) {
		_rows[index_row].y = row_position_begin;
		row_position_begin += _rows[index_row].height + line_width;
	}

	if (_rows.empty()) {
		return row_position_begin + border_width;
	} else {
		return row_position_begin + border_width - line_width;
	}
}

vector<_Table_Impl::Column>::iterator _Table_Impl::RecalculateColumnWidth(uint window_width) {
	auto it_column_width_changed = _columns.end();
	if (!GetStyle().auto_resize._width_auto_resize || window_width == GetSize().width) { 
		return it_column_width_changed;  // No need to auto resize or the height won't change.
	}

	for (auto it = _columns.rbegin(); it != _columns.rend(); ++it) {
		uint new_width = CalculateColumnWidth(GetStyle(), window_width, it->width_tag);
		if (new_width != it->width) {
			it->width = new_width;
			it_column_width_changed = it.base();
		}
	}
	if (it_column_width_changed == _columns.end()) { return; } // All columns' width remain unchanged, return directly.

	RecalculateColumnPosition(it_column_width_changed + 1); // Reset position from the next column.
	return it_column_width_changed; // Return the current column for resetting canvas for child windows.
}

vector<_Table_Impl::Row>::iterator _Table_Impl::RecalculateRowHeight(uint window_height) {  // Similar to above.
	auto it_row_height_changed = _rows.end();

	if (!GetStyle().auto_resize._height_auto_resize || window_height == GetSize().height) {
		return it_row_height_changed;
	}

	for (auto it = _rows.rbegin(); it != _rows.rend(); ++it) {
		uint new_height = CalculateRowHeight(GetStyle(), window_height, it->height_tag);
		if (new_height != it->height) {
			it->height = new_height;
			it_row_height_changed = it.base();
		}
	}
	if (it_row_height_changed == _rows.end()) { return; } // All rows' height remain unchanged, return directly.

	RecalculateRowPosition(it_row_height_changed + 1); // Reset position from the next row.
	return it_row_height_changed; // Return the current row for resetting canvas for child windows.
}

const _Table_Impl::HitTestGridInfo _Table_Impl::HitTestGrid(Point point, bool point_on_layer) const {
	uint border_width = GetStyle().border._width;
	Size size = point_on_layer ? GetLayerSize() : GetSize();
	if(point.x >= size.width || point.y >= size.height){ return { HitTestGridInfo::Type::OutSide , table_position_end , point };}
	if(point.x < border_width) { return { HitTestGridInfo::Type::BorderLeft , table_position_end , point }; }
	if(point.y < border_width) {return { HitTestGridInfo::Type::BorderTop , table_position_end , point };}
	if(point.x >= size.width - border_width) {return { HitTestGridInfo::Type::BorderRight , table_position_end , point };}
	if(point.y >= size.height - border_width) { return { HitTestGridInfo::Type::BorderDown , table_position_end , point }; }

	HitTestGridInfo::Type type = HitTestGridInfo::Type::Grid;

	// Find the last element that is less than or equal to the value.
	// Use reverse iterator, if no such element, return rend().
	auto cmp_row = [](const Row& row, uint y) { return row.y > y ? true : false; };
	auto it_row = std::lower_bound(_rows.rbegin(), _rows.rend(), point.y, cmp_row);
	if (it_row == _rows.rend()) { Error(); }				// Hit on the top border.
	if (point.y - it_row->y >= it_row->height) {
		if (it_row == _rows.rbegin()) { Error(); }			// Hit on the bottom border.
		type = HitTestGridInfo::Type::GridLineBottom;	// Hit on the bottom grid line.
	}
	auto cmp_column = [](const Column& column, uint x) { return column.x > x ? true : false; };
	auto it_column = std::lower_bound(_columns.rbegin(), _columns.rend(), point.x, cmp_column);
	if (it_column == _rows.rend()) { Error(); }				// Hit on the left border.
	if (point.x - it_column->x >= it_column->width) { 
		if (it_column == _columns.rbegin()) { Error(); }	// Hit on the right border.
		type = HitTestGridInfo::Type::GridLineRight;	// Hit on the right grid line.
	} 
	return HitTestGridInfo{
		type,
		TablePosition(it_row.base() - _rows.begin(), it_column.base() - _columns.begin()),
		point - (Point(it_column->x, it_row->y) - point_zero)
	};
}


void _Table_Impl::SetTableSize(TableSize size, ValueTag width, ValueTag height) {
	// Resize columns and rows.
	uint column_width = CalculateColumnWidth(GetStyle(), GetSize().width, width);
	if (width.IsAuto() && GetStyle().grid_size._width_fixed) { width.Set(column_width); }
	auto it_column = resize_with_element(_columns, size.column_number, Column(width, column_width));

	uint row_height = CalculateRowHeight(GetStyle(), GetSize().height, height);
	if (height.IsAuto() && GetStyle().grid_size._height_fixed) { height.Set(row_height); }
	auto it_row = resize_with_element(_rows, size.row_number, Row(height, row_height));

	// Resize child windows. Nullptr will be added, and trimmed child windows will destruct.
	_child_wnds.resize(size.column_number);
	for (auto& it : _child_wnds) { it.resize(size.row_number); }

	// Recalculate row and column position. But no need to reset child position.
	uint layer_height = RecalculateRowPosition(it_row);
	uint layer_width = RecalculateColumnPosition(it_column);

	// Resize the base layer and auto fit window size.
	UpdateLayerSize(Size(layer_height, layer_width));
}

void _Table_Impl::InsertColumn(uint column_begin, uint column_cnt, ValueTag width) {
	uint column_width = CalculateColumnWidth(GetStyle(), GetSize().width, width);
	if (width.IsAuto() && GetStyle().grid_size._width_fixed) { width.Set(column_width); }
	auto it_column = insert_from_begin_by_cnt(_columns, column_begin, column_cnt, Column(width, column_width));

	if (it_column == _columns.end()) { return; }

	insert_from_begin_by_cnt(_child_wnds, column_begin, column_cnt, vector<ChildContainer>(_rows.size()));

	uint layer_width = RecalculateColumnPosition(it_column);
	ResetChildPosition(it_column + column_cnt);

	// Resize the base layer and auto fit window size.

}

void _Table_Impl::InsertRow(uint row_begin, uint row_cnt, ValueTag height) {
	uint row_height = CalculateRowHeight(GetStyle(), GetSize().height, height);
	if (height.IsAuto() && GetStyle().grid_size._height_fixed) { height.Set(row_height); }
	auto it_row = insert_from_begin_by_cnt(_rows, row_begin, row_cnt, Row(height, row_height));

	if (it_row == _rows.end()) { return; }

	for (auto& it : _child_wnds) { insert_from_begin_by_cnt(it, row_begin, row_cnt, ChildContainer()); }

	uint layer_height = RecalculateRowPosition(it_row);
	ResetChildPosition(it_row + row_cnt);

	// Resize the base layer and auto fit window size.

}

void _Table_Impl::DeleteColumn(uint column_begin, uint column_cnt) {
	auto it_column = erase_from_begin_by_cnt(_columns, column_begin, column_cnt);
	if (it_column == _columns.end()) { return; }

	erase_from_begin_by_cnt(_child_wnds, column_begin, column_cnt);

	uint layer_width = RecalculateColumnPosition(it_column);
	ResetChildPosition(it_column);

	// Resize the base layer and auto fit window size.

}

void _Table_Impl::DeleteRow(uint row_begin, uint row_cnt) {
	auto it_row = erase_from_begin_by_cnt(_rows, row_begin, row_cnt);
	if (it_row == _rows.end()) { return; }

	for (auto& it : _child_wnds) { erase_from_begin_by_cnt(it, row_begin, row_cnt); }

	uint layer_height = RecalculateRowPosition(it_row);
	ResetChildPosition(it_row);

	// Resize the base layer and auto fit window size.

}

void _Table_Impl::SetColumnWidth(uint column, uint column_cnt, ValueTag width) {


}

void _Table_Impl::SetRowHeight(uint row, uint row_cnt, ValueTag height) {


}


void _Table_Impl::ResetChildPosition(vector<Column>::iterator it_column) {
	for (uint index_column = it_column - _columns.begin(); index_column < _columns.size(); ++index_column) {
		for (uint index_row = 0; index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			_layer.SetChildCanvas(
				*_child_wnds[index_column][index_row],
				Rect(_columns[index_column].x, _rows[index_row].y, _columns[index_column].width, _rows[index_row].height)
			);
		}
	}
}

void _Table_Impl::ResetChildPosition(vector<Row>::iterator it_row) {
	for (uint index_column = 0; index_column < _columns.size(); ++index_column) {
		for (uint index_row = it_row - _rows.begin(); index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			_layer.SetChildCanvas(
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
		for (uint index_row = it_row - _rows.begin(); index_row < _rows.size(); ++index_row) {
			if (_child_wnds[index_column][index_row] == nullptr) { continue; }
			_layer.SetChildCanvas(
				*_child_wnds[index_column][index_row],
				Rect(_columns[index_column].x, _rows[index_row].y, _columns[index_column].width, _rows[index_row].height)
			);
		}
	}
}

TablePosition _Table_Impl::GetChildPosition(Ref<_WndBase_Impl*> child) {
	HitTestGridInfo info = HitTestGrid(child->RegionOnLayer().point, true);
	if (info.type != HitTestGridInfo::Type::Grid) { Error(); }
	if (_child_wnds[info.grid.column][info.grid.row] != child) { Error(); } // Double check if the window is really the child.
	return info.grid;
}

bool _Table_Impl::SetChild(Ref<IWndBase*> wnd, TablePosition position) {

}

Ref<IWndBase*> _Table_Impl::GetChild(TablePosition position) const {
	uint column = position.column, row = position.row;
	if (column != column_end) {
		if (column >= _columns.size()) { return nullptr; }
		if (row != row_end) {
			if (row >= _rows.size()) { return nullptr; }
			return _child_wnds[column][row];
		}
		// Get the last non-null child window of the column.
		for (auto it = _child_wnds[column].rbegin(); it != _child_wnds[column].rend(); ++it) {
			if (*it != nullptr) { return *it; }
		}
		return nullptr;
	} else {
		if (row != row_end) {
			if (row >= _rows.size()) { return nullptr; }
			// Get the last non-null child window of the row.
			for (auto it = _child_wnds.rbegin(); it != _child_wnds.rend(); ++it) {
				if ((*it)[row] != nullptr) { return (*it)[row]; }
			}
			return nullptr;
		}
		// Get the last child window of the table.
		for (int index_row = _rows.size() - 1; index_row >= 0; index_row--) {  // Use int in case of overflowing at 0.
			for (int index_column = _columns.size() - 1; index_column >= 0; index_column--) {
				if (_child_wnds[index_column][index_row] != nullptr) {
					return _child_wnds[index_column][index_row];
				}
			}
		}
		return nullptr;
	}
}

bool _Table_Impl::RemoveChild(TablePosition position) {



}

bool _Table_Impl::UpdateChildRegion(Ref<_WndBase_Impl*> child) {
	// Get the child window's position.
	TablePosition grid = GetChildPosition(child);
	
	// If both row height and column width is fixed, return unchanged.
	if (_rows[grid.row].HeightFixed() && _columns[grid.column].WidthFixed()) { return false; }

	// Calculate the new size of child window, the point will be ignored. 
	// The current grid size is the parent size.
	const Size grid_size(_columns[grid.column].width, _rows[grid.row].height); // An alias for current grid size.
	Size required_size = child->CalculateRegionOnParent(grid_size).size;

	// Bound the size based on whether the size is fixed.
	if (_rows[grid.row].HeightFixed()) {
		required_size.height = grid_size.height; 
	} else if (_columns[grid.column].WidthFixed()) { // Use "else" because if height is fixed, then width must not be fixed.
		required_size.width = grid_size.width; 
	}
	// Bound the size between min and max grid size.
	required_size = BoundGridSize(GetStyle(), GetSize(), required_size);

	if (required_size == grid_size) { return false; } // Return if size is not changed.


	// If the required size is smaller than the current size, the column/row may shrink to fit the size, 
	//   but before that, all child windows' width of the same column/row will be calculated to make sure no 
	//   child window requires a larger size.
	// If the required size is larger than the current size, simply use the required size, because if there's another 
	//   child window of the same column/row who requires a larger size, the column/row's size must have already fitted to it.
	if (required_size.width < grid_size.width) {
		// If there are child windows of the same column who requires a larger width, increase the reqired width.
		for (uint index_row = 0; index_row < _rows.size(); ++index_row) {
			auto current_child = _child_wnds[grid.column][index_row];
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
			auto current_child = _child_wnds[index_column][grid.row];
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
	Size layer_size;
	if (grid_size.width != required_size.width) {
		it_column = _columns.begin() + grid.column;  // Point to current column.
		it_column->width = required_size.width;		 // Set the width of current column.
		layer_size.width = RecalculateColumnPosition(it_column + 1);  // Calculate the position from the next column to the end.
	}
	if (grid_size.height != required_size.height) {
		it_row = _rows.begin() + grid.row;
		it_row->height = required_size.height;
		layer_size.height = RecalculateRowPosition(it_row + 1);
	}

	// Reset canvas for subsequent child windows.
	ResetChildPosition(it_column, it_row);


	// Redraw the window.


	return true;
}


void _Table_Impl::Composite(Rect region_to_update) const {
	FigureQueue figure_queue;

	if (_layer.IsRedirected()) {  // No need to change coordinates for redirected layers.
		_layer.CompositeChild(region_to_update);
	} else {
		// No need to calculate intersected region.
		figure_queue.Push(
			LayerFigure::Create(_layer, _layer.ConvertToLayerPoint(region_to_update.point)),
			region_to_update
		);
	}

	DrawFigureQueue(figure_queue, region_to_update);
}

const Rect _Table_Impl::CalculateRegionOnParent(Size parent_size) const {
	TableStyle& style = const_cast<TableStyle&>(GetStyle());
	if (style.auto_resize.WillAutoResize()) {
		// Reset size to the layer size.
		Size layer_size = GetLayerSize();
		if (style.auto_resize._width_auto_resize) { style.size._normal.width.Set(layer_size.width); }
		if (style.auto_resize._height_auto_resize) { style.size._normal.height.Set(layer_size.height); }
		// The size will be bounded between min and max size when doing the next calculation.
	}
	Rect region_on_parent =  _WndBase_Impl::CalculateRegionOnParent(parent_size);  // The default size calculator.

	// If no auto resize, change the size of rows/columns according to the calculated size.
	auto it_row = RecalculateColumnWidth(region_on_parent.size.width);
	auto it_column
}

void _Table_Impl::SizeChanged() {
	// If does not auto resize, recalculate the size of columns and rows.
	auto it_column = RecalculateColumnWidth();
	auto it_row = RecalculateRowHeight();
	// Reset canvas for subsequent child windows if there's a column/row's size has changed.
	ResetChildPosition(it_column, it_row);
}


END_NAMESPACE(WndDesign)