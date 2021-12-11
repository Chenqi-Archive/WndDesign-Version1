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
	WNDDESIGN_API static unique_ptr<IWndBase, IWndBase::Deleter> 
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
	virtual void InsertRow(uint row_begin, uint row_cnt, ValueTag height = length_auto) pure;
	virtual void InsertColumn(uint column_begin, uint column_cnt, ValueTag width = length_auto) pure;

	// Delete multiple rows or columns at a time. 
	// If row_begin is bigger than the current row number, no row will be deleted.
	virtual void DeleteRow(uint row_begin, uint row_cnt) pure;
	virtual void DeleteColumn(uint column_begin, uint column_cnt) pure;

	// Set the width or height of the specified row or column.
	// If row_begin is bigger than the current row number, no row will be modified.
	virtual void SetRowHeight(uint row, uint row_cnt, ValueTag height = length_auto) pure;
	virtual void SetColumnWidth(uint column, uint column_cnt, ValueTag width = length_auto) pure;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
	// Set the child window at the position.
	// If there's already a child window at that position, the old child will be replaced by the current window,
	//   so you can specify the wnd to be nullptr to simply remove the old child.
	// You can add the child window to the end of a row or a column by specifying row_end or column_end. 
	//   The window will be inserted at the first position that was not taken up by a child window.
	//   If all positions are taken up, a new row or column is inserted.
	// In particular, if both row and column is end, the child window will be added to the end of the table.
	//   (That is, the last row's first empty position. If the row is full, a new row will be inserted.)
	// If there's no cooresponding position (the row is larger than the row number and is not row_end, the same for column), 
	//   the function fails and returns false.
	virtual bool SetChild(Ref<IWndBase*> wnd, TablePosition position = table_position_end) pure;

	// Retrieve the child window at the specified table position.
	// If row or column is row_end or column_end, returns the last child window of that row or column.
	// Returns nullptr if the position is empty or there's no cooresponding position.
	virtual Ref<IWndBase*> GetChild(TablePosition position) const pure;

	// Similar to GetChild.
	// If row or column is end, and there's only one child window left, the row or column will be also deleted.
	virtual bool RemoveChild(TablePosition position = table_position_end) pure;
};


END_NAMESPACE(WndDesign)