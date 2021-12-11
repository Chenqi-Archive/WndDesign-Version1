#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "layer.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)


class Canvas {
	//////////////////////////////////////////////////////////
	////                   Parent Layer                   ////
	//////////////////////////////////////////////////////////
private:
	// pointer to parent layer, may share the same layer base with other canvas.
	Ref Layer* _layer;
	Rect _region;	// relative to layer.
public:
	Canvas(Ref Layer* _layer, Rect _region);
	void Init(Ref Layer* _layer, Rect _region);
	Ref Layer* GetLayer() const { return _layer; }
	Rect GetRegion() const { return _region; }

	//////////////////////////////////////////////////////////
	////                 For Child Window                 ////
	//////////////////////////////////////////////////////////
public:
	Size GetSize() const;
	bool IsEmpty() const;

	// Get the visible region(can be seen) of the canvas.
	Rect GetVisibleRegion() const {
		return _layer->GetVisibleRegion().Intersect(_region) - (_region.point - point_zero);
	}


	///////////////////////////////////////////////////////////
	////                      Drawing                      ////
	///////////////////////////////////////////////////////////
public:
	void DrawFigureQueue(const queue<FigureContainer>& figure_queue)const {
		_layer->DrawFigureQueue(figure_queue, _region);
	}
};

END_NAMESPACE(WndDesign)