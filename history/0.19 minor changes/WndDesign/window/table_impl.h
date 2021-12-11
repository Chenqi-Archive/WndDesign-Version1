#pragma once

#include "table.h"
#include "wnd_base_impl.h"
#include "../render/table_layer.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


class _Table_Impl : virtual public ITable, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Table_Impl(Alloc<TableStyle*> style, Ref<ObjectBase*> object);
	~_Table_Impl() override {}


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	const TableStyle& GetTableStyle() const { return static_cast<const TableStyle&>(_WndBase_Impl::GetStyle()); }

	using _WndBase_Impl::GetSize;  // Get the size of the window.
	const Size GetLayerSize() const { return _layer.GetAccessibleRegion().size; } // Get the size of the layer.


	///////////////////////////////////////////////////////////
	////                       Table                       ////
	///////////////////////////////////////////////////////////
private:
	struct Row { 
		uint y;					// The start y position of the row. It is not specified on construction.
		uint height;			// The calculated row height. (y_n+1 - y_n == height_n + line_width)	
		ValueTag height_tag;	// The height tag used for calculating height. The height tag can be percent, or auto.
		explicit Row(ValueTag height_tag, uint height) : y(-1), height(height), height_tag(height_tag) {}
		bool HeightFixed() const { return !height_tag.IsAuto(); }
	};
	struct Column {  // similar to Row.
		uint x;					
		uint width;				
		ValueTag width_tag;
		explicit Column(ValueTag width_tag, uint width) : x(-1), width(width), width_tag(width_tag) {}
		bool WidthFixed() const { return !width_tag.IsAuto(); }
	};

	// The information of all columns and rows.
	vector<Row> _rows;
	vector<Column> _columns; 

private:
	// Helper functions to recalculate x and y positions, called when inserting or removing new rows/columns.
	// They will reset the x or y position from the first modified row to the end, and return the new size of the Table.
	// The size of all rows and columns, and the position of the previous row or column should already known.
	uint RecalculateColumnPosition(vector<Column>::iterator it_column);
	uint RecalculateRowPosition(vector<Row>::iterator it_row);

	// Called when table's size changed for not auto resizable table. 
	// Recalculate the width or height or all columns/rows based on table size, but not reset position.
	// Returns the first column whose width has changed, so the region of subsequent child windows will be reset.
	vector<Column>::iterator RecalculateColumnWidth(uint window_width);
	vector<Row>::iterator RecalculateRowHeight(uint window_height);

private:
	const TableHitTestInfo HitTestGridOnLayer(Point point) const;  // Point is relative to layer.
	const TableHitTestInfo HitTestGrid(Point point, bool point_on_layer) const override;

public:
	const TableSize GetTableSize() const override { 
		return TableSize(static_cast<uint>(_rows.size()), static_cast<uint>(_columns.size()));
	}

	void SetTableSize(TableSize size, ValueTag width, ValueTag height) override;

	void InsertColumn(uint column_begin, uint column_cnt, ValueTag width) override;
	void InsertRow(uint row_begin, uint row_cnt, ValueTag height) override;

	void DeleteColumn(uint column_begin, uint column_cnt) override;
	void DeleteRow(uint row_begin, uint row_cnt) override;

	void SetColumnWidth(uint column, uint column_cnt, ValueTag width) override;
	void SetRowHeight(uint row, uint row_cnt, ValueTag height) override;

private:
	// When the width of a column(or some columns) is changed, child windows may also auto resize, 
	//   (when setting the width of a column, or the columns resize when window resizes.)
	//   recalculate the row height accroding to the child window. 
	// Returns the first row that changed its size.
	vector<Row>::iterator AdjustRowHeightToColumnWidthChange();
	vector<Column>::iterator AdjustColumnWidthToRowHeightChange();  // similar to row.


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	// The child window table.
	// !!! The first index is column, the second is row. (_wnds[column][row]) !!!
	// The row number of each column must equal _rows.size().
	vector<vector<ChildContainer>> _child_wnds;

private:
	// Reset the position of child windows from it_row or it_column.
	// Used when inserting or deleting rows/columns. For most cases, just move the position of child windos but not change the size.
	void ResetChildPosition(vector<Column>::iterator it_column);
	void ResetChildPosition(vector<Row>::iterator it_row);
	// Reset the position of child windows from both it_row or it_column. 
	// Primarily used when a grid size has changed. The child windows' size of the row and the column should be changed.
	void ResetChildPosition(vector<Column>::iterator it_column, vector<Row>::iterator it_row);

	// Update the child's region at the calculated position. (child can be nullptr.)
	bool UpdateChildRegion(Ref<_WndBase_Impl*> child, TablePosition position);

private:
	// Get the TablePosition of child window.
	TablePosition GetChildPosition(_WndBase_Impl& child);

	// Set canvas for a child window.
	void SetChildCanvas(_WndBase_Impl& child, Rect region) { child.SetCanvas(Canvas(&_layer, region)); }

public:
	bool RemoveChild(Ref<IWndBase*> child_wnd) override {
		_WndBase_Impl* child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);
		if (child == nullptr) { return false; }
		return ITable::RemoveChild(GetChildPosition(*child));
	}

	bool SetChild(Ref<IWndBase*> child_wnd, TablePosition position) override;

	Ref<IWndBase*> GetChild(TablePosition position) const override {
		if (position.column >= _columns.size() || position.row >= _rows.size()) { return nullptr; }
		return _child_wnds[position.column][position.row];
	}


	bool UpdateChildRegion(Ref<_WndBase_Impl*> child_wnd) override {
		if (child_wnd == nullptr) { return false; }
		return UpdateChildRegion(child_wnd, GetChildPosition(*child_wnd));
	}

	void UpdateChildRenderStyle(Ref<_WndBase_Impl*> child_wnd) override { return; }  // Render style is ignored.


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	TableLayer _layer;
	friend class TableLayer;  // TableLayer may access the grid info or child windows when drawing.

private:
	// Called when grid changes and the layer need to enlarge or shrink.
	// The content beyond the unchanged_region will be redrawn. (unchanged_region's point is always point_zero.)
	// If window will auto resize, update the window size. (similar to _TextBox_Impl::TextUpdated())
	void LayerSizeUpdated(Size unchanged_region);

private:
	// Adjust the window size to the layer for auto resizable table.
	// These functions are called when parent window calculates my region, but it will really change the size.
	Size AdjustSizeToContent(Size min_size, Size max_size) override;
	uint AdjustHeightToContent(uint min_height, uint max_height, uint width) override;
	uint AdjustWidthToContent(uint min_width, uint max_width, uint height) override;

	// Canvas was reset and size is changed, recalculate grid size and reset canvas for child windows.
	void SizeChanged() override;


public:
	// The wnd's layer has refreshed, refresh the visible region for base layer.
	void RefreshVisibleRegion(Rect visible_region) override {
		_layer.RefreshCachedRegion(_layer.ConvertToLayerRegion(visible_region));
	}

	void Composite(Rect region_to_update) const override;

private:
	void ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) override {
		_layer.SetResourceManager(resource_manager);
	}


	///////////////////////////////////////////////////////////
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
public:
	const Point GetScrollPosition() const override { return _layer.GetCurrentPoint(); }
	void SetScrollPosition(Point point) override {
		if (!_layer.SetCurrentPoint(point)) {return;}
		BlockComposition();
		DispatchMessage(Msg::Scroll, nullptr);
		AllowComposition();
		RegionUpdated(region_infinite);	// Composite.
	}
	const Rect GetEntireRegion() const override { return _layer.GetAccessibleRegion(); }


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	Ref<_WndBase_Impl*> HitTestChild(Point point) const {
		TableHitTestInfo info = HitTestGrid(point, false);
		if (info.result == TableHitTestInfo::Result::Grid) {
			return _child_wnds[info.grid.column][info.grid.row];
		}
		return nullptr;
	}
};


END_NAMESPACE(WndDesign)