#pragma once

#include "../figure/figure.h"

BEGIN_NAMESPACE(WndDesign)

// The layer figure is used to draw layer on another layer.

class Layer;

class LayerFigure : public Figure {
private:
	static void Draw(const Figure& figure, Tile& tile, Rect region);
private:
	const Layer& _layer;
	Point _start_point;
public:
	LayerFigure(const Layer& layer, Point start_point):_layer(layer), _start_point(start_point), Figure(Draw) {}
};


END_NAMESPACE(WndDesign)