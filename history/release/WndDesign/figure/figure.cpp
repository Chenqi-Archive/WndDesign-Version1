#include "figure.h"

#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API void ColorFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, color); 
}

WNDDESIGN_API void BackgroundFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, background.GetColor());
	if (background.HasImage()) {
		tile.DrawImage(region, background.GetImage(), start_point_on_background,
					   0xFF, background.IsXRepeat(), background.IsYRepeat());
	}
}

WNDDESIGN_API void Line::DrawOn(Tile& tile, Rect region) const {
	tile.DrawLine(region.point + (start_point_offset - point_zero),
				  region.point + (end_point_offset - point_zero),
				  width,
				  color);
}

WNDDESIGN_API void Rectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRectangle(region, border_color, border_width, background);
}

WNDDESIGN_API void RoundedRectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRoundedRectangle(region, radius, border_color, border_width, background);
}

WNDDESIGN_API void TextFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawTextLayout(region, text_layout);
}


END_NAMESPACE(WndDesign)
