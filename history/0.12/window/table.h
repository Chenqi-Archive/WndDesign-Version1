#pragma once

#include "../common/core.h"
#include "../common/msg.h"

#include "wnd_base.h"

BEGIN_NAMESPACE(WndDesign)

// Table with the same line style.

struct TableStyle : WndStyle {
	struct GridLineStyle {
		uint width = 1px;
		Color32 color = ColorSet::Gray;
		void Set(uint width, Color32 color) { this->width = width; this->color = color; }
	}grid_line;

	uint default_column_width = 50px;
	uint default_row_height = 30px;
};


struct Table : virtual WndBase {
	static Alloc Table* Create(Ref Wnd* parent, const TableStyle& style, Handler handler = nullptr);

	virtual void SetStyle(const Ref TableStyle& style) pure;

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
	virtual bool AddChild(WndBase* wnd, TablePosition position = size_auto) pure;
	// If position is not set, remove the last child window.
	virtual bool RemoveChild(TablePosition position = size_auto) pure;
	virtual bool RemoveChild(WndBase* wnd) pure;
};

END_NAMESPACE(WndDesign)
