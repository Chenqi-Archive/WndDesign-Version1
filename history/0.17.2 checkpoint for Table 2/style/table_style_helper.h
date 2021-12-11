#pragma once

#include "table_style.h"
#include "style_helper.h"


BEGIN_NAMESPACE(WndDesign)


// Called at the construction of Table to set grid size to default size if size_normal is auto.
inline void CheckGridSize(TableStyle& style) {
	TableStyle::GridSize& grid_size = style.grid_size;
	if (grid_size._normal.height.IsAuto()) { grid_size._normal.height = grid_size.grid_size_default.height; }
	if (grid_size._normal.width.IsAuto()) { grid_size._normal.width = grid_size.grid_size_default.width; }
}


inline uint CalculateRowHeight(const TableStyle& style, uint window_height, ValueTag height) {
	TableStyle::GridSize grid_size = style.grid_size;
	if (height.IsAuto()) { height = grid_size._normal.height; }  // Now height must not be auto.
	if (height.IsPercent()) {
		if (style.auto_resize._height_auto_resize) { ExceptionDialog(style_parse_exception); }
		height.ConvertToPixel(window_height);
	}
	grid_size._min.height.ConvertToPixel(window_height);
	grid_size._max.height.ConvertToPixel(window_height);
	BoundSizeBetween(height, grid_size._min.height, grid_size._max.height);
	return height.AsUnsigned();
}

inline uint CalculateColumnWidth(const TableStyle& style, uint window_width, ValueTag width) {
	TableStyle::GridSize grid_size = style.grid_size;
	if (width.IsAuto()) { width = grid_size._normal.width; }  // Now width must not be auto.
	if (width.IsPercent()) {
		if (style.auto_resize._width_auto_resize) { ExceptionDialog(style_parse_exception); }
		width.ConvertToPixel(window_width);
	}
	grid_size._min.width.ConvertToPixel(window_width);
	grid_size._max.width.ConvertToPixel(window_width);
	BoundSizeBetween(width, grid_size._min.width, grid_size._max.width);
	return width.AsUnsigned();
}

inline Size BoundGridSize(const TableStyle& style, Size window_size, Size required_size) {
	TableStyle::GridSize grid_size = style.grid_size;
	ValueTag height(required_size.height), width(required_size.width);
	grid_size._min.height.ConvertToPixel(window_size.height);
	grid_size._min.width.ConvertToPixel(window_size.width);
	grid_size._max.height.ConvertToPixel(window_size.height);
	grid_size._max.width.ConvertToPixel(window_size.width);
	BoundSizeBetween(height, grid_size._min.height, grid_size._max.height);
	BoundSizeBetween(width, grid_size._min.width, grid_size._max.width);
	return Size(width.AsUnsigned(), height.AsUnsigned());
}


END_NAMESPACE(WndDesign)