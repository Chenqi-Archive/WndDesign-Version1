#pragma once

#include "color.h"

#include "../geometry/geometry.h"
#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


class Figure {
private:
	using DrawFunc = void(*)(const Figure& figure, Tile& tile, Rect region);
	DrawFunc function;
public:
	Figure(DrawFunc function) :function(function) {}
	void DrawOn(Tile& tile, Rect region) const { function(*this, tile, region); }
};


class Color : public Figure {
private:
	static void Draw(const Figure& figure, Tile& tile, Rect region) {
		const Color& object = static_cast<const Color&>(figure);
		tile.DrawColor(region, object.color);
	}
private:
	Color32 color;
public:
	Color(Color32 color) :color(color), Figure(Draw) {}
};

class Rectangle : public Figure {
private:
	static void Draw(const Figure& figure, Tile& tile, Rect region) {
		const Rectangle& object = static_cast<const Rectangle&>(figure);
		tile.DrawRectangle(region, object.border_color, object.border_width, object.fill_color);
	}
private:
	Color32 border_color;
	Color32 fill_color;  // May use brushes instead.
private:
	uint border_width;

public:
	Rectangle(uint border_width, Color32 border_color, Color32 fill_color) :
		border_width(border_width), border_color(border_color), fill_color(fill_color), Figure(Draw) {
	}
};


class RoundedRectangle : public Figure {
private:
	static void Draw(const Figure& figure, Tile& tile, Rect region) {
		const RoundedRectangle& object = static_cast<const RoundedRectangle&>(figure);
		tile.DrawRoundedRectangle(region, object.radius, object.border_color, object.border_width, object.fill_color);
	}
private:
	Color32 border_color;
	Color32 fill_color;
	uint border_width;
	uint radius;

public:
	RoundedRectangle(uint radius, uint border_width, Color32 border_color, Color32 fill_color) :
		radius(radius), border_width(border_width), border_color(border_color), fill_color(fill_color), Figure(Draw) {
	}
};


class TextFigure : public Figure {
private:
	static void Draw(const Figure& figure, Tile& tile, Rect region) {
		const TextFigure& object = static_cast<const TextFigure&>(figure);
		tile.DrawTextLayout(region, object.text_layout);
	}
private:
	const TextLayout& text_layout;
public:
	TextFigure(const TextLayout& text_layout) :
		text_layout(text_layout), Figure(Draw) {
	}
};


END_NAMESPACE(WndDesign)