#pragma once

#include "../WndDesign.h"


// The widget for tracking selection region(relative to base layer) when mouse clicks and drags. 
//   (Used by SpaceView)
class MouseDragSelectionTracker {
private:
	ScrollWnd& _parent;

	inline static Color selection_color = Color(ColorSet::Gray, 0x7F);
	bool _has_mouse_down = false;
	Point _mouse_down_position = point_zero;
	Rect _selection_region = region_empty;  // relative to base layer.

private:
	void UpdateSelectionRegion(Rect selection_region) {
		if (_selection_region != selection_region) {
			Rect union_region = selection_region.Union(_selection_region);
			_selection_region = selection_region;
			union_region.point = _parent->ConvertBaseLayerPointToPoint(union_region.point);
			_parent->UpdateRegion(union_region);
		}
	}

public:
	MouseDragSelectionTracker(ScrollWnd& parent) : _parent(parent) {}

	const Rect GetRegion() const { return _selection_region; }

	void OnLeftDown(Point position) {
		_has_mouse_down = true;
		_mouse_down_position = _parent->ConvertPointToBaseLayerPoint(position);
		_parent->SetCapture();
	}

	void OnMouseMove(Point position) {
		if (_has_mouse_down) {
			// Scroll the base layer if position falls near the border.
			Point bounded_point = BoundPointInRegion(position, ShrinkRegionByLength(Rect(point_zero, _parent->GetSize()), 30));
			if (Vector vector = position - bounded_point; vector != vector_zero) {
				_parent->ScrollView(vector);
			}

			position = _parent->ConvertPointToBaseLayerPoint(position);
			Point left_top(min(_mouse_down_position.x, position.x), min(_mouse_down_position.y, position.y));
			Point right_down(max(_mouse_down_position.x, position.x), max(_mouse_down_position.y, position.y));
			UpdateSelectionRegion(Rect(left_top, Size(uint(right_down.x - left_top.x), uint(right_down.y - left_top.y))));
		}
	}

	const Rect OnLeftUp(Point position) {
		if (_has_mouse_down) {
			_has_mouse_down = false;
			Rect selection = _selection_region;
			UpdateSelectionRegion(region_empty);
			_parent->ReleaseCapture();
			return selection;
		}
		return region_empty;
	}

	// Used for drawing selection region.
	void AppendFigure(FigureQueue& figure_queue) {
		if (!_selection_region.IsEmpty()) {
			figure_queue.append(
				new ColorFigure(selection_color),
				Rect(_parent->ConvertBaseLayerPointToPoint(_selection_region.point), _selection_region.size)
			);
		}
	}
};