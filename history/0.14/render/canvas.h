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
	explicit Canvas(Ref Layer* layer = nullptr, Rect region = region_empty) :
		_layer(layer), _region(region) {
	}

	bool IsEmpty() const { return _region.IsEmpty(); }
	Size GetSize() const { return _region.size; }

	// Get the visible region on the canvas.
	Rect GetVisibleRegion() const {
		if (_layer == nullptr) { return region_empty; }
		return _layer->GetCachedRegion().Intersect(_region) - (_region.point - point_zero);
	}

	const Ref Layer* GetLayer() const { return _layer; }

	const Point ConvertToParentPoint(Point point) const {
		if (_layer == nullptr) { return point_zero; }
		return _layer->ConvertToParentPoint(point + (_region.point - point_zero));
	}

public:
	// A region on the window is updated, inform the layer to recomposite child windows.
	void RegionUpdated(Rect region) const {
		if (_layer == nullptr) { return; }
		_layer->RegionUpdated(_region.Intersect(region + (_region.point - point_zero)), true);
	}

	// Draw a queue of figures inside the region(relative to the canvas).
	// Forward to the layer.
	void DrawFigureQueue(const FigureQueue& figure_queue,
						 Rect bounding_region,
						 Vector position_offset) const {
		if (_layer == nullptr) { return; }
		_layer->DrawFigureQueue(figure_queue,
								_region.Intersect(bounding_region + (_region.point - point_zero)),
								_region.point - point_zero + position_offset);
	}
};

END_NAMESPACE(WndDesign)