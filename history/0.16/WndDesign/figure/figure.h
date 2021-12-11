#pragma once

#include "background.h"


BEGIN_NAMESPACE(WndDesign)


// Tile class declaration. Tile is a target that will do actual rendering.
class Tile;


// The Figure abstract base class.
// Figure serves as a container for commiting drawing instructions to a tile.
// The DrawOn() virtual function is used to draw different shapes. 

// Note: "region" is relative to the tile, which may be different from tiles to tiles, and may 
//   be larger or smaller than the size of a tile. But the size of the region will not change, 
//   which is determined by the size of a Wnd who issues the drawing instructions.
class Figure {
public:
	virtual void DrawOn(Tile& tile, Rect region) const pure;
	// virtual ~Figure();   // Not used. A figure object should NOT contain any allocated resource.
};


// Some basic figures are presented below. 
// You could implement your own figures base on these simple figures.
// A figure object should NOT contain any allocated resource, no pointer should be used, use reference only.


class ColorFigure : public Figure {
private:
	Color color;
public:
	ColorFigure(Color color) :color(color) {}
	void DrawOn(Tile& tile, Rect region) const override;
};


// Used for redirected layer to draw background at position offset.
class BackgroundFigure : public Figure {
private:
	const Background& background;
	Point start_point_on_background;
public:
	BackgroundFigure(const Background& background, Point start_point_on_background) :
		background(background), start_point_on_background(start_point_on_background){}
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
	const Background& background;   // Use reference because background may contain allocated resources(image).
	uint border_width;
	Color border_color;

public:
	Rectangle(uint border_width, Color border_color, const Background& background) :
		border_width(border_width), border_color(border_color), background(background) {
	}
	void DrawOn(Tile& tile, Rect region) const override;
};


class RoundedRectangle : public Figure {
private:
	const Background& background;
	uint border_width;
	Color border_color;
	uint radius;

public:
	RoundedRectangle(uint radius, uint border_width, Color border_color, const Background& background) :
		radius(radius), border_width(border_width), border_color(border_color), background(background) {
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


END_NAMESPACE(WndDesign)