#pragma once

#include "texture.h"


BEGIN_NAMESPACE(WndDesign)


// Tile class declaration. Tile is a target that will do actual rendering.
class Tile;


// The Figure abstract base class.
// Figure serves as a container for commiting drawing instructions to a tile.
// The DrawOn() virtual function is used to draw different shapes. 
// Note region is relative to the tile, which may be different from tiles to tiles, and may 
//   be larger or smaller than the size of a tile. But the size of the region will not change, 
//   which is determined by the size of a Wnd who issues the drawing instructions.
class Figure {
public:
	virtual void DrawOn(Tile& tile, Rect region) const pure;
	// virtual ~Figure();   // Not used. A figure object should NOT contain any allocated resource.
};


// You can reference the max size of a tile to bound the region in case the region is too large.
// For example, when drawing grid lines on a large ScrollWnd's base layer, you cannot enumerate 
//   all lines, only several lines will actually hit the tile. See GridLine below.
constexpr Size max_tile_size = Size(1024, 1024);



// Some basic figures are presented below. 
// You could implement your own figures base on these simple figures.
// A figure object should NOT contain any allocated resource, no pointer should be used, use reference only.


// As a wrapper to reference an allocated figure to avoid copying and destruction.
class FigureReference : public Figure {
private:
	const Figure& figure;
public:
	FigureReference(const Figure& figure) :figure(figure) {}
	void DrawOn(Tile& tile, Rect region) const override;
};


class SolidColor : public Figure {
private:
	Color color;
public:
	SolidColor(Color color) :color(color) {}
	void DrawOn(Tile& tile, Rect region) const override;
};


class Line : public Figure {
private:
	Point start_point_offset, end_point_offset;
	Color color;
	uint width;

public:
	// Point offset is relative to the origin point of the drawing region.
	// The part falls out of the region will be clipped.
	Line(Point start_point_offset, Point end_point_offset, Color color, uint width) :
		start_point_offset(start_point_offset), end_point_offset(end_point_offset), color(color), width(width) {
	}
	void DrawOn(Tile& tile, Rect region) const override;
};


class Rectangle : public Figure {
private:
	Color fill_color;  // May use brushes instead.
	uint border_width;
	Color border_color;

public:
	Rectangle(uint border_width, Color border_color, Color fill_color) :
		border_width(border_width), border_color(border_color), fill_color(fill_color) {
	}
	void DrawOn(Tile& tile, Rect region) const override;
};


class RoundedRectangle : public Figure {
private:
	Color fill_color;
	uint border_width;
	Color border_color;
	uint radius;

public:
	RoundedRectangle(uint radius, uint border_width, Color border_color, Color fill_color) :
		radius(radius), border_width(border_width), border_color(border_color), fill_color(fill_color) {
	}
	void DrawOn(Tile& tile, Rect region) const override;
};


class Image;

class ImageFigure : public Figure {
private:
	const Image& image;
	Point start_position;
	enum class FillStyle { None, Repeat, Stretch } x_style, y_style;
	
public:
	ImageFigure(const Image& image, 
				FillStyle x_style = FillStyle::Repeat, 
				FillStyle y_style = FillStyle::Repeat, 
				Point start_position = point_zero) :
		image(image), x_style(x_style), y_style(y_style), start_position(start_position) {
	}
	void DrawOn(Tile& tile, Rect region) const override;
};


class TextLayout;

class TextFigure : public Figure {
private:
	const TextLayout& text_layout;
public:
	TextFigure(const TextLayout& text_layout) :text_layout(text_layout) {}
	void DrawOn(Tile& tile, Rect region) const override;
};


// A customized figure example using basic figures listed above.
class GridLine : public Figure {
private:
	Color line_color;
	uint line_width;
	uint x_period, y_period;  // The x and y repeat period, not including line width.
	Color background_color;
public:
	GridLine(Color line_color, uint line_width, uint x_period, uint y_period, Color background_color = color_transparent) :
		line_color(line_color), line_width(line_width), x_period(x_period), y_period(y_period), background_color(background_color) {
	}
	void DrawOn(Tile& tile, Rect region) const override {
		// First clear the tile with background color.
		if (background_color != color_transparent) {
			SolidColor color(background_color);
			color.DrawOn(tile, region);
		}

		// Find the starting grid line position. 
		// x_start = k * x_period where k is the minimum interger that satisfies: region.point.x + k * x_period >= 0.
		auto find_start_position = [](int begin_position, uint period) -> int {
			if (begin_position >= 0) { return 0; }
			return (begin_position / static_cast<int>(period)) * static_cast<int>(period);
		};
		int x_start = find_start_position(region.point.x, x_period);
		int y_start = find_start_position(region.point.y, y_period);
		int x_end = static_cast<int>(max_tile_size.width) - region.point.x;
		int y_end = static_cast<int>(max_tile_size.height) - region.point.y;

		// Draw vertical lines.
		for (int x = x_start; x <= x_end; x += static_cast<int>(x_period)) {
			Point start_point(x, y_start), end_point(x, y_end);
			Line line(start_point, end_point, line_color, line_width);
			line.DrawOn(tile, region);
		}
		// Draw horizontal lines.
		for (int y = y_start; y <= y_end; y += static_cast<int>(y_period)) {
			Point start_point(x_start, y), end_point(x_end, y);
			Line line(start_point, end_point, line_color, line_width);
			line.DrawOn(tile, region);
		}
	}
};


END_NAMESPACE(WndDesign)