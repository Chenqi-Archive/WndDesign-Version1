#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


// A convenient tool to translate mouse dragging messages to moving vectors.
// You are expected to use it as a member variable of your Wnd class.
class MouseDragTracker {
private:
	ObjectBase& _parent;
	bool _has_mouse_down;
	Point _mouse_down_position;

public:
	MouseDragTracker(ObjectBase& parent) : _parent(parent), _has_mouse_down(false), _mouse_down_position(point_zero) {}

	void SetDownPosition(Point mouse_down_position) {
		_mouse_down_position = mouse_down_position;
	}

	// Returns the moving vector if there is dragging, or return vector_zero.
	Vector TrackMsg(Msg msg, Para para) {
		if (!IsMouseMsg(msg)) { return vector_zero; }
		MouseMsg mouse_msg = GetMouseMsg(para);
		if (msg == Msg::LeftDown) {
			_has_mouse_down = true;
			_mouse_down_position = mouse_msg.point;
			_parent->SetCapture();
			return vector_zero;
		}
		if (msg == Msg::LeftUp) {
			_has_mouse_down = false;
			_parent->ReleaseCapture();
			return vector_zero;
		}
		if (msg == Msg::MouseMove) {
			if (!_has_mouse_down) { return vector_zero; }
			Point current_position = mouse_msg.point;
			Vector moving_vector = current_position - _mouse_down_position;
			return moving_vector;  // moving_vector could also be zero.
		}
		return vector_zero;
	}
};


END_NAMESPACE(WndDesignHelper)