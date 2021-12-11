#pragma once

#include "wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


// Table with uniform grid line style.
// If the size of the table is auto, the table will auto resize depending on row, column and child windows.
// If the size is fixed or exceeding the max size, the table will be scrollable.
struct TableStyle : WndStyle {

	// Determines if the table's size will automatically adjust when content changes.
	// Note that the size will still be bounded between size_min and size_max.
	// If size_normal is not auto, the auto resize styles will be ignored.
	struct AutoResize {
	public:
		bool _width_auto_resize = false;
		bool _height_auto_resize = false;
		bool WillAutoResize() const { return _width_auto_resize || _height_auto_resize; }
	public:
		constexpr void set(bool width_auto_resize, bool height_auto_resize) {
			_width_auto_resize = width_auto_resize; _height_auto_resize = height_auto_resize;
		}
	}auto_resize;


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


	// The inner grid line style.
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


constexpr uint row_end = -1;
constexpr uint column_end = -1;
constexpr TablePosition table_position_end = TablePosition(row_end, column_end);


END_NAMESPACE(WndDesign)