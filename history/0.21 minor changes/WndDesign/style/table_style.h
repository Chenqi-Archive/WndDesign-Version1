#pragma once

#include "wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


// Table with uniform grid line style.
struct TableStyle : WndStyle {

	// Specifies the default, minium and maximun grid size.
	// The default size may be auto change to fit the size of child window, and will be bounded by max and min size.
	//   but if you set the width or height to be fixed, it will not auto fit size.
	struct GridSize : public SizeStyle{
	public:
		static constexpr SizeTag grid_size_default = { 100px, 30px };
		bool _width_fixed = false;
		bool _height_fixed = false;
	public:
		constexpr GridSize& setWidthFixed() { _width_fixed = true; return *this; }
		constexpr GridSize& setHeightFixed() { _height_fixed = true; return *this; }
	}grid_size;


	// The uniform inner grid line style.
	struct GridLineStyle {
	public:
		Color _color = ColorSet::Gray;
		uint _width = 1;
	public:
		constexpr GridLineStyle& width(uint width) { _width = width; return *this; }
		constexpr GridLineStyle& color(Color color) { _color = color; return *this; }
	}grid_line;
};


struct TableSize {
	uint column_number;
	uint row_number;
	explicit constexpr TableSize(uint column_number, uint row_number) :
		column_number(column_number), row_number(row_number) {
	}
};


struct TablePosition {
	uint column;
	uint row;
	explicit constexpr TablePosition(uint column, uint row) :
		column(column), row(row) {
	}
};


// Used for specifying the end of the row/column.
constexpr uint column_end = -1;
constexpr uint row_end = -1;

// The invalid table position.
constexpr TablePosition table_position_invalid = TablePosition(-1, -1);


// The return structure for hit-testing a table.
struct TableHitTestInfo {
	enum class Result {
		OutSide,			// grid == table_position_invalid	point == the original point
		BorderLeft,			// ... (The same as above)
		BorderTop,			// ...
		BorderRight,		// ...
		BorderDown,			// ...
		Grid,				// grid == the grid hit				point == point relative to the grid
		GridLineRight,		// grid == the left grid			...
		GridLineBottom,		// grid == the top grid				...
	} result;
	TablePosition grid;
	Point point;
};


END_NAMESPACE(WndDesign)