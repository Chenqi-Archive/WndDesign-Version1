#include "layer_figure.h"
#include "layer.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<LayerFigure> LayerFigure::Create(const Layer& layer, Point start_point) {
	return unique_ptr<LayerFigure>(new LayerFigure(layer, start_point));
}

void LayerFigure::DrawOn(Tile& tile, Rect region) const {
	layer.DrawOn(start_point, tile, region);
}


END_NAMESPACE(WndDesign)