#pragma once

#include "../WndDesign.h"
#include "../helper.h"


// The widget for tracking mouse dragging and scroll the view. (Used by SpaceView)
class MouseDragViewScroller {
private:
	ScrollWnd& _parent;
	
	bool _has_mouse_down = false;
	Point _mouse_down_position = point_zero;

public:
	MouseDragViewScroller(ScrollWnd& parent) : _parent(parent) {}

	void OnLeftDown(Point position) {
		_has_mouse_down = true;
		_mouse_down_position = position;
		_parent->SetCapture();
	}

	bool OnMouseMove(Point position) {
		if (_has_mouse_down){
			Vector moving_vector = position - _mouse_down_position;
			if (moving_vector != vector_zero) {
				SetCursor(Cursor::Move);
				Point old_scroll_position = _parent->GetScrollPosition();
				Point new_scroll_position = old_scroll_position - moving_vector;
				new_scroll_position = BoundPointToNearestLatticePoint(new_scroll_position, Size(16, 16));
				if (new_scroll_position != old_scroll_position) {
					_parent->SetScrollPosition(new_scroll_position);
					moving_vector = old_scroll_position - _parent->GetScrollPosition();
					_mouse_down_position = _mouse_down_position + moving_vector;
				}
				return true;
			}
		}
		return false;
	}

	void OnLeftUp() {
		if (_has_mouse_down) {
			_has_mouse_down = false;
			_parent->ReleaseCapture();
		}
	}

	void AppendFigure(FigureQueue& figure_queue) {

	}
};