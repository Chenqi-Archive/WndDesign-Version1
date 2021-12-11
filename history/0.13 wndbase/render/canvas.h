#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "layer.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)


class Canvas {
private:
	// pointer to parent layer, may share the same layer base with other canvases.
	Ref Layer* _layer;
	Rect _region;	// relative to layer.

public:
	// Only layer could create canvases.
	Canvas(Ref Layer* layer = nullptr, Rect region = region_empty):
		_layer(layer), _region(region){ }

	void Init(Ref Layer* layer, Rect region) {
		_layer = layer;
		_region = region;
	}

	bool IsEmpty() const { return _layer == nullptr || _region.IsEmpty(); }
	Size GetSize() const { return _region.size; }

	// Get the visible region on the canvas.
	Rect GetVisibleRegion() const {
		return _layer->GetVisibleRegion().Intersect(_region) - (_region.point - point_zero);
	}

	const Ref Layer* GetLayer() const { return _layer; }

	const Point ConvertToParentPoint(Point point) const {
		return _layer->ConvertToParentPoint(point + (_region.point - point_zero));
	}

public:
	// Draw a queue of figures inside the region(relative to the canvas).
	// Forward to the layer.
	// Returns the updated region on the canvas.
	Rect DrawFigureQueue(FigureQueue& figure_queue,
						 Vector position_offset = vector_zero,
						 Rect bounding_region = region_infinite,
						 bool commit_immediately = true) const {
		Rect bounding_region_on_layer = _region.Intersect(bounding_region + (_region.point - point_zero));
		Rect updated_region_on_layer = _layer->DrawFigureQueue(figure_queue,
															   _region.point - point_zero + position_offset,
															   bounding_region_on_layer,
															   commit_immediately);
		return _region.Intersect(updated_region_on_layer) - (_region.point - point_zero);
	}
};

END_NAMESPACE(WndDesign)