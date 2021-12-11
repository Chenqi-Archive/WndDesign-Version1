#include "figure.h"

#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


void ColorFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, color);
}

void BackgroundFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, background.GetColor());
	if (background.HasImage()) {
		tile.DrawImage(region, background.GetImage(), start_point_on_background,
					   0xFF, background.IsXRepeat(), background.IsYRepeat());
	}
}

void Line::DrawOn(Tile& tile, Rect region) const {
	tile.DrawLine(region.point + (start_point_offset - point_zero),
				  region.point + (end_point_offset - point_zero),
				  width,
				  color);
}

void Rectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRectangle(region, border_color, border_width, background);
}

void RoundedRectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRoundedRectangle(region, radius, border_color, border_width, background);
}

void TextFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawTextLayout(region, text_layout);
}


END_NAMESPACE(WndDesign)
