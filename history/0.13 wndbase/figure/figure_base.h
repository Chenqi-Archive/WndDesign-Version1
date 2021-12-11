#pragma once

#include "color.h"

#include "../geometry/geometry.h"
#include "../render/tile.h"


BEGIN_NAMESPACE(WndDesign)


class Figure {
public:
	virtual void DrawOn(Tile& tile, Rect region) const pure;
	// virtual ~Figure() pure {}  // No need.
};



class Color : public Figure {
private:
	Color32 color;
public:
	Color(Color32 color) :color(color) {}
	void DrawOn(Tile& tile, Rect region) const override {
		tile.DrawColor(region, color);
	}
};


class Rectangle : public Figure {
private:
	Color32 border_color;
	Color32 fill_color;
	// May use brushes instead.
private:
	uint border_width;

public:
	Rectangle(uint border_width, Color32 border_color, Color32 fill_color) :
		border_width(border_width), border_color(border_color), fill_color(fill_color) {}
	void DrawOn(Tile& tile, Rect region) const override {
		tile.DrawRectangle(region, border_color, border_width, fill_color);
	}
};


class RoundedRectangle : public Figure {
private:
	Color32 border_color;
	Color32 fill_color;
	uint border_width;
	uint radius;

public:
	RoundedRectangle(uint radius, uint border_width, Color32 border_color, Color32 fill_color) :
		radius(radius), border_width(border_width), border_color(border_color), fill_color(fill_color) {}
	void DrawOn(Tile& tile, Rect region) const override {
		tile.DrawRoundedRectangle(region, radius, border_color, border_width, fill_color);
	}
};


class Text : public Figure {
private:
	const wstring& text;
	const TextBoxStyle& style;
public:
	Text(const wstring& text, const TextBoxStyle& style) :
		text(text), style(style) {}
	void DrawOn(Tile& tile, Rect region) const override {

	}
};


END_NAMESPACE(WndDesign)