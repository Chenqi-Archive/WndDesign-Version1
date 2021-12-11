#include "figure.h"

#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<ColorFigure> ColorFigure::Create(Color color) {
	return unique_ptr<ColorFigure>(new ColorFigure(color));
}
void ColorFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, color);
}

WNDDESIGN_API unique_ptr<BackgroundFigure> BackgroundFigure::Create(const Background& background, Point start_point_on_background) {
	return unique_ptr<BackgroundFigure>(new BackgroundFigure(background, start_point_on_background));
}
void BackgroundFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawColor(region, background.GetColor());
	if (background.HasImage()) {
		tile.DrawImage(region, background.GetImage(), start_point_on_background,
					   0xFF, background.IsXRepeat(), background.IsYRepeat());
	}
}

WNDDESIGN_API unique_ptr<Line> Line::Create(Point start_point_offset, Point end_point_offset, Color color, uint width) {
	return unique_ptr<Line>(new Line(start_point_offset, end_point_offset, color, width));
}
void Line::DrawOn(Tile& tile, Rect region) const {
	tile.DrawLine(region.point + (start_point_offset - point_zero),
				  region.point + (end_point_offset - point_zero),
				  width,
				  color);
}

WNDDESIGN_API unique_ptr<Rectangle> Rectangle::Create(uint border_width, Color border_color, const Background& background) {
	return unique_ptr<Rectangle>(new Rectangle(border_width, border_color, background));
}
void Rectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRectangle(region, border_color, border_width, background);
}

WNDDESIGN_API unique_ptr<RoundedRectangle> RoundedRectangle::Create(uint radius, uint border_width, Color border_color, const Background& background) {
	return unique_ptr<RoundedRectangle>(new RoundedRectangle(radius, border_width, border_color, background));
}
void RoundedRectangle::DrawOn(Tile& tile, Rect region) const {
	tile.DrawRoundedRectangle(region, radius, border_color, border_width, background);
}

WNDDESIGN_API unique_ptr<TextFigure> TextFigure::Create(const TextLayout& text_layout) {
	return unique_ptr<TextFigure>(new TextFigure(text_layout));
}
void TextFigure::DrawOn(Tile& tile, Rect region) const {
	tile.DrawTextLayout(region, text_layout);
}


END_NAMESPACE(WndDesign)
