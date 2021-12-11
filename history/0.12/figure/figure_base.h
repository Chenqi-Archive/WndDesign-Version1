#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../render/render_target.h"


BEGIN_NAMESPACE(WndDesign)


class Figure {
public:
	virtual ~Figure() pure {}
	virtual void DrawOn(RenderTarget* render_target, Rect region) const pure;
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
	void DrawOn(RenderTarget* render_target, Rect region) const {
		render_target->DrawRectangle(region, border_color, );
	}
};


class RoundedRectangle : public Figure {
private:
	Color32 border_color;
	Color32 fill_color;

private:
	uint border_width;
	uint radius;
public:
	RoundedRectangle(uint radius, uint border_width, Color32 border_color, Color32 fill_color) :
		radius(radius), border_width(border_width), border_color(border_color), fill_color(fill_color) {}
	void DrawOn(RenderTarget* render_target, Rect region) const {
		render_target->DrawRectangle();
	}
};


class TextBox : public Figure {
public:
	void DrawOn(RenderTarget* render_target, Rect region) const {
		render_target->DrawText();
	}
};


END_NAMESPACE(WndDesign)