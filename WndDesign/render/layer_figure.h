#pragma once

#include "../figure/figure.h"


BEGIN_NAMESPACE(WndDesign)


// The layer figure is used to draw layer on another layer when compositing.

class Layer;

class LayerFigure : public Figure {
private:
	const Layer& layer;
	Point start_point;
public:
	LayerFigure(const Layer& layer, Point start_point):layer(layer), start_point(start_point) {}
	void DrawOn(Tile& tile, Rect region) const override {
		layer.DrawOn(start_point, tile, region);
	}
};


END_NAMESPACE(WndDesign)