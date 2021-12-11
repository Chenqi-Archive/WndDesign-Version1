#pragma once

#include "table.h"
#include "wnd_base_impl.h"
#include "../render/table_layer.h"

#include <vector>
#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::vector;
using std::list;


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

private:
	struct Column {
		uint x;			  // The start x position of the column.
		uint width;       // The column width. (x_n+1 - x_n == width_n + line_width)
		bool is_width_fixed;   // If true, the width will not auto change with child windows.

		explicit Column(uint width, bool is_width_fixed) : 
			x(-1), width(width), is_width_fixed(is_width_fixed) {  // x value will be calculated later.
		}  
	};
	struct Row {
		uint y;
		uint height;
		bool is_height_fixed;
		vector<ChildWndContainer> wnds;

		explicit Row(uint height, bool is_height_fixed, uint column_number) :
			y(-1), height(height), is_height_fixed(is_height_fixed), wnds(column_number) {
		}
	};
	vector<Column> _columns;  // The x-position and width of all columns.
	list<Row> _rows;		  // The y-position, height and child window list of all rows.

private:
	// Helper functions to recalculate x and y positions.
	// These functions will reset the x or y position from the iterator to the end,
	//   reset child windows' canvas and return the new size of the Table.
	// The size of all rows and columns, and the position of the previous row or column should already known.
	uint RecalculateRowPosition(list<Row>::iterator it_row);
	uint RecalculateColumnPosition(vector<Column>::iterator it_column);

public:
	const TableSize GetTableSize() const override { return TableSize(_rows.size(), _columns.size()); }
	void SetTableSize(TableSize size) override;

	void InsertRow(uint row_begin, uint row_cnt, ValueTag height = length_auto) override;
	void InsertColumn(uint column_begin, uint column_cnt, ValueTag width = length_auto) override;

	void DeleteRow(uint row_begin, uint row_cnt) override;
	void DeleteColumn(uint column_begin, uint column_cnt) override;

	void SetRowHeight(uint row, uint row_cnt, ValueTag height = length_auto) override;
	void SetColumnWidth(uint column, uint column_cnt, ValueTag width = length_auto) override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
//private:
//	Ref<ChildWndContainer*> GetChildByPosition(TablePosition position);
//	Ref<ChildWndContainer*> GetChildByPointer(Ref<IWndBase*> child_wnd);

public:
	bool AddChild(Ref<IWndBase*> child_wnd) override { return SetChild(child_wnd, table_position_end);}
	bool SetChild(Ref<IWndBase*> wnd, TablePosition position) override;

	Ref<IWndBase*> GetChild(TablePosition position) const override;

	bool RemoveChild(Ref<IWndBase*> child_wnd) override;
	bool RemoveChild(TablePosition position) override;

	// Called by child window whose style has just changed.
	bool UpdateChildRegion(Ref<IWndBase*> child_wnd) override;
	void UpdateChildRenderStyle(Ref<IWndBase*> child_wnd) override;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	TableLayer _layer;

public:
	// The wnd's layer has refreshed, refresh the visible region for child layers.
	void RefreshVisibleRegion(Rect visible_region) override;

	void Composite(Rect region_to_update) const override;

	// The region on the layer has updated, the window has to recomposite.
	// The composite operation may be masked or delayed.
	void LayerUpdated(Ref<const Layer*> layer, Rect region) const override;

private:
	void ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) override;

	// Set the canvas, allocate canvases for child windows, and composite.
	void SizeChanged() override;


	// Functions below are called redirectedly by table_layer, because layer has no information of
	//   child windows.
public:
	void RefreshChildVisibleRegion(Rect cached_region) const;

	void RedrawChild(Rect region_to_update) const;

	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager);


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)