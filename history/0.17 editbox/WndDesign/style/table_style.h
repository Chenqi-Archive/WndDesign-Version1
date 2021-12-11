#pragma once

#include "wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


// Table with the same line style.

struct TableStyle : WndStyle {
	// Specifies the default, minium and maximun grid size.
	SizeStyle grid_size;

	struct GridLineStyle {
	private:
		Color _color = ColorSet::Gray;
		uint _width = 1;
	public:
		constexpr GridLineStyle& width(uint width) { _width = width; return *this; }
		constexpr GridLineStyle& color(Color color) { _color = color; return *this; }
	}grid_line;
};



struct TableSize {
	uint row_number;
	uint column_number;
	explicit constexpr TableSize(uint row_number, uint column_number) :
		row_number(row_number), column_number(column_number) {
	}
};


struct TablePosition {
	uint row;
	uint column;
	explicit constexpr TablePosition(uint row, uint column) :
		row(row), column(column) {
	}
};


constexpr TablePosition table_position_end = TablePosition(-1, -1);


END_NAMESPACE(WndDesign)