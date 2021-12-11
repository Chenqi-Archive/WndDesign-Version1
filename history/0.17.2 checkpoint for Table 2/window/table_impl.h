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
	const TableStyle& GetStyle() const { return static_cast<const TableStyle&>(_WndBase_Impl::GetStyle()); }

	using _WndBase_Impl::GetSize;  // Get the size of the window.
	const Size GetLayerSize() const { _layer.GetAccessibleRegion().size; } // Get the size of the layer.

public:
	// If the region is set explicitly, the auto_resize styles should be removed,
	// The size of rows/columns will be recalculated when reset canvas and SizeChanged() is called.
	bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) override;

private:
	// Called when grid changes and the layer need to enlarge or shrink.
	// If window will auto resize, update the window size. (Similar to _TextBox_Impl::TextUpdated().)
	void UpdateLayerSize(Size layer_size);

private:
	struct Row { 
		uint y;					// The start y position of the row. It is not specified on construction.
		uint height;			// The calculated row height. (y_n+1 - y_n == height_n + line_width)	
		ValueTag height_tag;	// The height tag used for calculating height. The height tag can be percent, or auto.
		explicit Row(ValueTag height_tag, uint height) : y(-1), height(height), height_tag(height_tag) {}
		bool HeightFixed() const { return !height_tag.IsAuto(); }
	};
	struct Column {  // The same as row.
		uint x;					
		uint width;				
		ValueTag width_tag;
		explicit Column(ValueTag width_tag, uint width) : x(-1), width(width), width_tag(width_tag) {}
		bool WidthFixed() const { return !width_tag.IsAuto(); }
	};
	vector<Row> _rows;		  // The y-position and height of all rows.
	vector<Column> _columns;  // The x-position and width of all columns.

private:
	// Helper functions to recalculate x and y positions, called when inserting or removing new rows/columns.
	// They will reset the x or y position from the first modified row to the end, and return the new size of the Table,
	//   but the width or height should already be known.
	// The size of all rows and columns, and the position of the previous row or column should already known.
	uint RecalculateColumnPosition(vector<Column>::iterator it_column);
	uint RecalculateRowPosition(vector<Row>::iterator it_row);

	// Called when table's size change for not auto resizable table. 
	// Recalculate the width or height or all columns/rows based on parent size, and then call recalculate position to reset position.
	// Returns the first column whose width has changed, so the region of subsequent child windows will be reset.
	vector<Column>::iterator RecalculateColumnWidth(uint window_width);
	vector<Row>::iterator RecalculateRowHeight(uint window_height);

	struct HitTestGridInfo {
		enum class Type {
			OutSide,			// grid == table_position_end	offset == point
			BorderLeft,			// grid == table_position_end	offset == point
			BorderTop,			// grid == table_position_end	offset == point
			BorderRight,		// grid == table_position_end	offset == point
			BorderDown,			// grid == table_position_end	offset == point
			Grid,				// grid == (the grid hit)		offset == point - grid_position
			GridLineRight,		// grid == (the left grid)      offset == point - grid_position
			GridLineBottom,		// grid == (the top grid)		offset == point - grid_position
		} type;
		TablePosition grid;
		Point offset;
	};

	// Hit test position in pixel to get row or column number.
	// The point may be relative to table or base layer.
	// For unscrollable table, it's the same, but for scrollable table, the layer region is always larger than the window region.
	const HitTestGridInfo HitTestGrid(Point point, bool point_on_layer) const;

public:
	const TableSize GetTableSize() const override { return TableSize(_rows.size(), _columns.size()); }
	void SetTableSize(TableSize size, ValueTag width, ValueTag height) override;

	void InsertColumn(uint column_begin, uint column_cnt, ValueTag width) override;
	void InsertRow(uint row_begin, uint row_cnt, ValueTag height) override;

	void DeleteColumn(uint column_begin, uint column_cnt) override;
	void DeleteRow(uint row_begin, uint row_cnt) override;

	void SetColumnWidth(uint column, uint column_cnt, ValueTag width) override;
	void SetRowHeight(uint row, uint row_cnt, ValueTag height) override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	// The child window table.
	// !!! The first index is column, the second is row. (_wnds[column][row]) !!!
	// The row number of each column must equal the row number.
	vector<vector<ChildContainer>> _child_wnds;

private:
	// Reset the position of child windows from it_row or it_column.
	// Used when inserting or deleting rows/columns. Just move the position of child windos but not change the size.
	void ResetChildPosition(vector<Column>::iterator it_column);
	void ResetChildPosition(vector<Row>::iterator it_row);
	// Reset the position of child windows from both it_row or it_column. 
	// Used when a grid size has changed. The child windows' size of the row and the column should be changed.
	void ResetChildPosition(vector<Column>::iterator it_column, vector<Row>::iterator it_row);

private:
	// Get the TablePosition of child window by referencing child's region on layer.
	TablePosition GetChildPosition(Ref<_WndBase_Impl*> child);

public:
	bool AddChild(Ref<IWndBase*> child_wnd) override { return SetChild(child_wnd, table_position_end);}
	bool SetChild(Ref<IWndBase*> wnd, TablePosition position) override;

	Ref<IWndBase*> GetChild(TablePosition position) const override;

	bool RemoveChild(Ref<IWndBase*> child_wnd) override {
		return RemoveChild(GetChildPosition(dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd)));
	}
	bool RemoveChild(TablePosition position) override;

	// Called by child window whose style has just changed.
	bool UpdateChildRegion(Ref<_WndBase_Impl*> child_wnd) override;
	void UpdateChildRenderStyle(Ref<_WndBase_Impl*> child_wnd) override { return; }  // Render style is ignored.


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	TableLayer _layer;
	friend class TableLayer;  // TableLayer may access the grid info or child windows when drawing.

public:
	// The wnd's layer has refreshed, refresh the visible region for base layer.
	void RefreshVisibleRegion(Rect visible_region) override {
		_layer.RefreshCachedRegion(_layer.ConvertToLayerRegion(visible_region));
	}

	void Composite(Rect region_to_update) const override;

	// Callback by parent window to calculate size. Similar to TextBox.
	// Table may auto fit with layer.
	const Rect CalculateRegionOnParent(Size parent_size) const override;

private:
	void ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) override {
		_layer.SetResourceManager(resource_manager);
	}

	// Canvas was reset and size is changed, recalculate grid size and reset canvas for child windows.
	void SizeChanged() override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)