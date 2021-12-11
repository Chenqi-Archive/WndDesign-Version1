#include "layer_figure.h"
#include "layer.h"

BEGIN_NAMESPACE(WndDesign)

void LayerFigure::Draw(const Figure& figure, Tile& tile, Rect region) {
	const LayerFigure& object = static_cast<const LayerFigure&>(figure);
	object._layer.DrawOn(object._start_point, tile, region);
}

END_NAMESPACE(WndDesign)

