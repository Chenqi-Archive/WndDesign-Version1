#pragma once

#include "../WndDesign.h"
#include "../helper.h"


// The widget for tracking mouse dragging and scroll the view. (Used by SpaceView)
class MouseDragViewScroller {
private:
	ScrollWnd& _parent;

	inline static const uint scroll_check_period = 40;  //ms
	Timer _timer;

	bool _has_mouse_down = false;
	Point _mouse_down_position = point_zero;
	Point _mouse_current_position = point_zero;
	bool _is_dragging = false;

	inline static const Color center_circle_color = ColorSet::Gray;
	inline static const uint center_circle_radius = 20;
	inline static const Color floating_circle_color = ColorSet::Gray;
	inline static const uint floating_circle_radius = 10;

public:
	MouseDragViewScroller(ScrollWnd& parent) : _parent(parent) , _timer(parent) {}

	void OnLeftDown(Point position) {
		_has_mouse_down = true;
		_mouse_down_position = position;
		_parent->SetCapture();
	}

	bool OnMouseMove(Point position) {
		if (_has_mouse_down) {
			if (!_is_dragging) {
				_is_dragging = true;
				_parent->UpdateRegion(BoundingRegionOfCircle(_mouse_down_position, center_circle_radius));
			}
			_parent->UpdateRegion(BoundingRegionOfCircle(_mouse_current_position, floating_circle_radius));
			_mouse_current_position = position;
			_parent->UpdateRegion(BoundingRegionOfCircle(_mouse_current_position, floating_circle_radius));
			if (SquareLength(_mouse_current_position - _mouse_down_position) >= square(center_circle_radius)) {
				if (!_timer.IsSet()) {
					_timer.Set(scroll_check_period);
				}
				_parent->ScrollView((_mouse_current_position - _mouse_down_position) * 0.1f);
			} else {
				_timer.Stop();
			}
		}
		return false;
	}

	void OnLeftUp() {
		if (_has_mouse_down) {
			_has_mouse_down = false;
			_parent->ReleaseCapture();
		}
		if (_is_dragging) {
			_is_dragging = false;
			_parent->UpdateRegion(BoundingRegionOfCircle(_mouse_down_position, center_circle_radius));
			_parent->UpdateRegion(BoundingRegionOfCircle(_mouse_current_position, floating_circle_radius));
			_timer.Stop();
		}
	}

	void OnTimer(Timer* timer) {
		if (timer == &_timer) {
			_parent->ScrollView((_mouse_current_position - _mouse_down_position) * 0.1f);
		}
	}

	void AppendFigure(FigureQueue& figure_queue) {
		if (_has_mouse_down) {
			figure_queue.append(
				new RoundedRectangle(center_circle_radius, 1, ColorSet::Gold, background_transparent), 
				BoundingRegionOfCircle(_mouse_down_position, center_circle_radius)
			);
			figure_queue.append(
				new RoundedRectangle(floating_circle_radius, 1, ColorSet::DarkGreen, background_transparent),
				BoundingRegionOfCircle(_mouse_current_position, floating_circle_radius)
			);
		}
	}
};