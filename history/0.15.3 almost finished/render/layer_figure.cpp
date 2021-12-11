#include "layer_figure.h"
#include "layer.h"

BEGIN_NAMESPACE(WndDesign)

void LayerFigure::DrawOn(Tile& tile, Rect region) const {
	layer.DrawOn(start_point, tile, region);
}

END_NAMESPACE(WndDesign)