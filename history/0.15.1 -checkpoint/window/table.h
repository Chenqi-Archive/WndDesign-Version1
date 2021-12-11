#pragma once

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

// Table with the same line style.

struct TableStyle : WndStyle {
	struct GridLineStyle {
	private:
		Color _color = ColorSet::Gray;
		ushort _width = 1;
	public:
		constexpr GridLineStyle& width(ushort width) { _width = width; return *this; }
		constexpr GridLineStyle& color(Color color) { _color = color; return *this; }
	}grid_line;

	uint default_column_width = 50;
	uint default_row_height = 30;
};


struct Table : virtual IWndBase {
	static Alloc<Table*> Create(Ref<IWnd*> parent, const TableStyle& style, Ref<ObjectBase*> object = nullptr);

	virtual void SetStyle(const TableStyle& style) pure;

	using TableSize = Size;
	using TablePosition = Size;

	virtual void SetSize(TableSize size) pure;
	virtual void InsertColumn(uint column_number, uint width) pure;
	virtual void InsertRow(uint row_number, uint height) pure;
	virtual void SetColumnWidth(uint column_number, uint width) pure;
	virtual void SetRowHeight(uint row_number, uint height) pure;
	virtual void DeleteColumn(uint column_number) pure;
	virtual void DeleteRow(uint row_number) pure;

	// Set the child window at the position, if the position is not set, add the window at the end. 
	// The child window's position and size is fixed, regardless its style.
	virtual bool AddChild(IWndBase* wnd, TablePosition position) pure;
	// If position is not set, remove the last child window.
	virtual bool RemoveChild(TablePosition position) pure;
	virtual bool RemoveChild(IWndBase* wnd) pure;
};

END_NAMESPACE(WndDesign)
