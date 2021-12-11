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
	static Alloc<ITable*> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object = nullptr);

	using TableSize = Size;
	using TablePosition = Size;

	virtual void SetTableSize(TableSize size) pure;
	virtual void InsertColumn(uint column_number, ValueTag width) pure;
	virtual void InsertRow(uint row_number, ValueTag height) pure;
	virtual bool DeleteColumn(uint column_number) pure;
	virtual bool DeleteRow(uint row_number) pure;
	virtual bool SetColumnWidth(uint column_number, ValueTag width) pure;
	virtual bool SetRowHeight(uint row_number, ValueTag height) pure;

	// Set the child window at the position.
	virtual bool AddChild(Ref<IWndBase*> wnd, TablePosition position) pure;
	// If position is not set, remove the last child window.
	virtual bool RemoveChild(TablePosition position) pure;
};

END_NAMESPACE(WndDesign)
