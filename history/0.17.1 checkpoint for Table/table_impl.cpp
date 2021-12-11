#include "table_impl.h"
#include "../style/table_style_helper.h"
#include "../common/stl_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWndBase, IWndBase::Deleter> 
ITable::Create(Ref<IWndBase*> parent, const TableStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<ITable, IWndBase::Deleter> table(new _Table_Impl(new TableStyle(style), object));
	table->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(table.get()); }
	return table;
}



_Table_Impl::_Table_Impl(Alloc<TableStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object), _layer(*this) {
	CheckGridSize(*style);
}


uint _Table_Impl::RecalculateRowPosition(list<Row>::iterator it_row) {
	uint border_width = GetStyle().border._width;
	uint line_width = GetStyle().grid_line._width;
	uint row_position;
	if (it_row == _rows.begin()) {
		row_position = border_width;
	} else {
		--it_row;
		row_position = it_row->y + it_row->height + line_width;
		++it_row;
	}
	for (; it_row != _rows.end(); ++it_row) {
		it_row->y = row_position;
		row_position += it_row->height + line_width;
		for (int index_wnd = 0; index_wnd < _columns.size(); index_wnd++) {
			if (it_row->wnds[index_wnd] != nullptr) {
				_layer.SetChildCanvas(
					*it_row->wnds[index_wnd],
					Rect(Point(_columns[index_wnd].x, it_row->y), Size(_columns[index_wnd].width, it_row->height))
				);
			}
		}
	}
	return row_position - line_width + border_width;
}

uint _Table_Impl::RecalculateColumnPosition(vector<Column>::iterator it_column) {
	uint border_width = GetStyle().border._width;
	uint line_width = GetStyle().grid_line._width;
	uint column_position;
	
	uint index_column_begin = it_column - _columns.begin();
	if (index_column_begin == 0) {
		column_position = border_width;
	} else {
		column_position = _columns[index_column_begin - 1].x + _columns[index_column_begin - 1].width + line_width;
	}
	for (uint index_column = index_column_begin; index_column < _columns.size(); ++index_column) {
		_columns[index_column].x = column_position;
		column_position += _columns[index_column].width + line_width;
	}
	for (auto& it_row : _rows) {
		for (uint index_wnd = index_column_begin; index_wnd < _columns.size(); ++index_wnd) {
			if (it_row.wnds[index_wnd] != nullptr) {
				_layer.SetChildCanvas(
					*it_row.wnds[index_wnd],
					Rect(Point(_columns[index_wnd].x, it_row.y), Size(_columns[index_wnd].width, it_row.height))
				);
			}
		}
	}
	return column_position - line_width + border_width;
}

void _Table_Impl::SetTableSize(TableSize size) {
	// Resize the rows.
	if (_rows.size() > size.row_number) { DeleteRow(size.row_number, _rows.size() - size.row_number); }
	if (_rows.size() < size.row_number) { InsertRow(_rows.size(), size.row_number - _rows.size()); }
	// Resize the columns.
	if (_columns.size() > size.column_number) { DeleteColumn(size.column_number, _columns.size() - size.column_number); }
	if (_columns.size() < size.column_number) { InsertColumn(_columns.size(), size.column_number - _columns.size()); }
}

void _Table_Impl::InsertRow(uint row_begin, uint row_cnt, ValueTag height) {
	// Calculate default height.
	uint row_height = CalculateRowHeight(GetStyle(), GetSize().height, height);

	// Insert rows.
	auto it_row = emplace_from_begin_by_cnt(
		_rows, row_begin, row_cnt,
		row_height, GetStyle().grid_size._height_fixed, _columns.size()
	);
	if (it_row == _rows.end()) { return; } // No row was inserted.

	// Calculate y value from the first modified row, and reset canvas for child windows.
	uint window_height = RecalculateRowPosition(it_row);

	// Resize the window.

}

void _Table_Impl::InsertColumn(uint column_begin, uint column_cnt, ValueTag width) {
	// Calculate default width.
	uint column_width = CalculateColumnWidth(GetStyle(), GetSize().width, width);

	// Insert columns.
	auto it_column = emplace_from_begin_by_cnt(
		_columns, column_begin, column_cnt,
		column_width, GetStyle().grid_size._width_fixed
	);
	if (it_column == _columns.end()) { return; }  // No column was inserted.

	// Modify the column number of all rows.
	for (auto& it_row : _rows) {
		emplace_from_begin_by_cnt(it_row.wnds, column_begin, column_cnt);
	}

	// Calculate x value.
	uint window_width = RecalculateColumnPosition(it_column);

	// Resize the window.

}

void _Table_Impl::DeleteRow(uint row_begin, uint row_cnt) {
	auto it_row = erase_from_begin_by_cnt(_rows, row_begin, row_cnt);
	if (it_row == _rows.end()) { return; }

	// Calculate y value from the first modified row, and reset canvas for child windows.
	uint window_height = RecalculateRowPosition(it_row);

	// Resize the window.

}

void _Table_Impl::DeleteColumn(uint column_begin, uint column_cnt) {
	auto it_column = erase_from_begin_by_cnt(_columns, column_begin, column_cnt);
	if (it_column == _columns.end()) { return; }

	// Modify all rows and reset canvas for child windows.
	for (auto& it_row : _rows) {
		erase_from_begin_by_cnt(it_row.wnds, column_begin, column_cnt);
	}

	// Calculate x value.
	uint window_width = RecalculateColumnPosition(it_column);

	// Resize the window.
}

void _Table_Impl::SetRowHeight(uint row, uint row_cnt, ValueTag height) {
	// Get the row iterator.
	auto it_row = get_iterator_by_index(_rows, row);
	if (it_row == _rows.end()) { return; }

	// Calculate row height.
	uint row_height = CalculateRowHeight(GetStyle(), GetSize().height, height);

	// If height is not auto, set the row height as fixed, or set it to be auto resizable.
	if (height.IsAuto()) {

	} else {

	}

	// Auto fit height with child windows.

	// Recalculate y value and set canvas.

	// Resize the window.

}

void _Table_Impl::SetColumnWidth(uint column, uint column_cnt, ValueTag width) {
	// Get the column iterator.
	auto it_column = get_iterator_by_index(_columns, column);
	if (it_column == _columns.end()) { return; }

	// Calculate column width.
	uint row_height = CalculateRowHeight(GetStyle(), GetSize().width, width);

	// If width is not auto, set the column width as fixed, or set it to be auto resizable.
	if (width.IsAuto()) {

	} else {

	}

	// Auto fit width with child windows.

	// Recalculate x value and set canvas.

	// Resize the window.

}


bool _Table_Impl::SetChild(Ref<IWndBase*> wnd, TablePosition position) {
	if (position.row == row_end) {

	}
	auto it_row = get_iterator_by_index(_rows, position.row);


	return false;
}

Ref<IWndBase*> _Table_Impl::GetChild(TablePosition position) const {
	if (position.row != row_end) {
		auto it_row = get_iterator_by_index(_rows, position.row);
		if (it_row == _rows.end()) { return nullptr; }
		if (position.column != column_end) {
			auto it_wnd = get_iterator_by_index(it_row->wnds, position.column);
			if (it_wnd == it_row->wnds.end()) { return nullptr; }
			return *it_wnd;
		} else {
			for (auto it_wnd = it_row->wnds.rbegin(); it_wnd != it_row->wnds.rend(); ++it_wnd) {
				if (*it_wnd != nullptr) { return *it_wnd; }
			}
			return nullptr;
		}
	} else {
		if (position.column != column_end) {
			if (position.column >= _columns.size()) { return nullptr; }
			for (auto it_row = _rows.rbegin(); it_row != _rows.rend(); ++it_row) {
				if (it_row->wnds[position.column] != nullptr) { return it_row->wnds[position.column]; }
			}
			return nullptr;
		} else {
			for (auto it_row = _rows.rbegin(); it_row != _rows.rend(); ++it_row) {
				for (auto it_wnd = it_row->wnds.rbegin(); it_wnd != it_row->wnds.rend(); ++it_wnd) {
					if (*it_wnd != nullptr) { return *it_wnd; }
				}
			}
			return nullptr;
		}
	}
}

bool _Table_Impl::RemoveChild(Ref<IWndBase*> child_wnd) {
	for (auto it_row = _rows.begin(); it_row != _rows.end(); ++it_row) {
		for (auto it_wnd = it_row->wnds.begin(); it_wnd != it_row->wnds.end(); ++it_wnd) {
			if (*it_wnd != child_wnd) { continue; }

			it_wnd->Clear();// Clear the window container.

			// Clear the base layer.

			return true;
		}
	}
	return false;
}

bool _Table_Impl::RemoveChild(TablePosition position) {  // Similar to GetChild().
	if (position.row != row_end) {
		auto it_row = get_iterator_by_index(_rows, position.row);
		if (it_row == _rows.end()) { return false; }
		if (position.column != column_end) {
			auto it_wnd = get_iterator_by_index(it_row->wnds, position.column);
			if (it_wnd == it_row->wnds.end()) { return false; }
			it_wnd->Clear();

			// Clear the base layer.

		} else {
			for (auto it_wnd = it_row->wnds.rbegin(); it_wnd != it_row->wnds.rend(); ++it_wnd) {
				if (*it_wnd != nullptr) {
					it_wnd->Clear();

					if (it_wnd.base() == it_row->wnds.begin()) {
						// The only child window of the row has been removed, remove the row.

					}

					// Clear the base layer.
				}
			}
			return false;
		}
	} else {
		if (position.column != column_end) {
			if (position.column >= _columns.size()) { return false; }
			for (auto it_row = _rows.rbegin(); it_row != _rows.rend(); ++it_row) {
				if (it_row->wnds[position.column] != nullptr) { 
					it_row->wnds[position.column].Clear();

					if (it_row.base() == _rows.begin()) {
						// The only child window of the column has been removed, remove the column.

					}

					// 
				}
			}
			return false;
		} else {
			for (auto it_row = _rows.rbegin(); it_row != _rows.rend(); ++it_row) {
				for (auto it_wnd = it_row->wnds.rbegin(); it_wnd != it_row->wnds.rend(); ++it_wnd) {
					if (*it_wnd != nullptr) {
						it_wnd->Clear();

						if (it_wnd.base() == it_row->wnds.begin()) {
							// The only child window of the row has been removed, remove the row.

						}

						//
					}
				}
			}
			return false;
		}
	}
}


END_NAMESPACE(WndDesign)