#pragma once

#include "wnd_base.h"
#include "../style/table_style.h"


BEGIN_NAMESPACE(WndDesign)


AbstractInterface ITable : virtual IWndBase {
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~ITable() {}

public:
	using StyleType = TableStyle;
	WNDDESIGN_API static unique_ptr<ITable, IWndBase::Deleter>
		Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object = nullptr);


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	// Force resize the table. Ususally used for initialization.
	// If the size is larger than original size, new grids will use the specified size, if not specified(length_auto), 
	//   default grid size is used.
	// If the size is smaller than original size, the exceeding child windows will be removed.
	virtual void SetTableSize(TableSize size, ValueTag column_width = length_auto, ValueTag row_height = length_auto) pure;
	virtual const TableSize GetTableSize() const pure;

	// Insert multiple rows or columns at a time. You can specify the height or not.
	// If row_begin is equal to or bigger than the current row number, the row will be inserted at the end.
	virtual void InsertColumn(uint column_begin, uint column_cnt, ValueTag width = length_auto) pure;
	virtual void InsertRow(uint row_begin, uint row_cnt, ValueTag height = length_auto) pure;

	// Delete multiple rows or columns at a time. 
	// If row_begin is bigger than the current row number, no row will be deleted.
	virtual void DeleteColumn(uint column_begin, uint column_cnt) pure;
	virtual void DeleteRow(uint row_begin, uint row_cnt) pure;

	// Set the width or height of the specified row or column. The width or height should not be auto.
	// If row_begin is bigger than the current row number, no row will be modified.
	virtual void SetColumnWidth(uint column, uint column_cnt, ValueTag width) pure;
	virtual void SetRowHeight(uint row, uint row_cnt, ValueTag height) pure;


	// For hit-testing a point in pixel to get row or column number. 
	// The point may be relative to relative to the window or the layer. 
	//   (It differs for scrollable table, whose layer might be larger than window.)
	virtual const TableHitTestInfo HitTestGrid(Point point, bool point_on_layer) const pure;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Set the child window at the position, if there's already a child window, it will be replaced by the new one.
	//   If the position is invalid(falls out of the table), the function fails and returns false.
	// It is the ONLY function to add a child window, it's useless to specify Table as parent or use the default AddChild().
	virtual bool SetChild(Ref<IWndBase*> child_wnd, TablePosition position) pure;

	// Get the child window at the position.
	// Returns nullptr if there's no cooresponding position.
	virtual Ref<IWndBase*> GetChild(TablePosition position) const pure;

	// Remove the child window at the position by setting a nullptr at the position.
	bool RemoveChild(TablePosition position) { return SetChild(nullptr, position); }
};


END_NAMESPACE(WndDesign)