#pragma once

#include "../figure/figure_base.h"

BEGIN_NAMESPACE(WndDesign)

// The layer figure is used to draw layer on another layer.

class Layer;

class LayerFigure : public Figure {
	const Layer& _layer;
	Point _start_point;

public:
	LayerFigure(const Layer& layer, Point start_point):_layer(layer), _start_point(start_point) {}
	void DrawOn(Tile& tile, Rect region) const override;
};


END_NAMESPACE(WndDesign)