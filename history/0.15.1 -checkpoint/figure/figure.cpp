#include "figure.h"

#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


void FigureReference::DrawOn(Tile& tile, Rect region) const {
	figure.DrawOn(tile, region);
}

void SolidColor::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, color);
}

void Line::DrawOn(Tile& tile, Rect region) const {
	tile.DrawLine(region.point + (start_point_offset - point_zero),
				  region.point + (end_point_offset - point_zero),
				  width,
				  color);
}

void Rectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRectangle(region, border_color, border_width, fill_color);
}

void RoundedRectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRoundedRectangle(region, radius, border_color, border_width, fill_color);
}

void TextFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawTextLayout(region, text_layout);
}


END_NAMESPACE(WndDesign)