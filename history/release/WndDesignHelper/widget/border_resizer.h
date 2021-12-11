#pragma once

#include "../WndDesign.h"
#include "../style/border_position.h"


BEGIN_NAMESPACE(WndDesignHelper)


// A convenient tool to resize or move window by dragging border.
// !!! It doesn't work for "static" child windows of ScrollWnd. !!!
class BorderResizer {
public:
	BorderResizer(ObjectBase& parent) :
		_parent(parent),
		_has_mouse_down(false),
		_mouse_down_position(point_zero),
		_mouse_down_border_position(BorderPosition::None),
		_mouse_down_window_region(region_empty) {
	}

private:
	ObjectBase& _parent;

private:
	bool _has_mouse_down;  // true if mouse hit on border when left button down.
	Point _mouse_down_position;  // The mouse_down position relative to parent window.
	BorderPosition _mouse_down_border_position;
	Rect _mouse_down_window_region;  // The window's region relative to parent window.

private:
	inline bool ResetRightPosition(WndStyle& style, int x_delta) {
		if (x_delta == 0) { return false; }
		// Keep left unchanged in pixel, reset the width.
		// The new width might be smaller than the min_width and even negative, never mind! 
		// It will be bounded to min_width when calculating the actual region on parent.
		style.position._left = px(_mouse_down_window_region.point.x);
		style.size._normal.width = px(static_cast<int>(_mouse_down_window_region.size.width) + x_delta);
		style.auto_resize._width_auto_resize = false; // Clear the auto_resize style.
		return true;
	}
	inline bool ResetDownPosition(WndStyle& style, int y_delta) {
		if (y_delta == 0) { return false; }
		style.position._top = px(_mouse_down_window_region.point.y);
		style.size._normal.height = px(static_cast<int>(_mouse_down_window_region.size.height) + y_delta);
		style.auto_resize._height_auto_resize = false;
		return true;
	}
	inline bool ResetLeftPosition(WndStyle& style, int x_delta, uint parent_width) {
		if (x_delta == 0) { return false; }
		// Keep right unchanged in pixel, set left to be auto, reset the width.
		style.position._left = position_auto;
		style.position._right = px(static_cast<int>(parent_width) - _mouse_down_window_region.RightBottom().x);
		style.size._normal.width = px(static_cast<int>(_mouse_down_window_region.size.width) - x_delta);
		style.auto_resize._width_auto_resize = false;
		return true;
	}
	inline bool ResetTopPosition(WndStyle& style, int y_delta, uint parent_height) {
		if (y_delta == 0) { return false; }
		style.position._top = position_auto;
		style.position._down = px(static_cast<int>(parent_height) - _mouse_down_window_region.RightBottom().y);
		style.size._normal.height = px(static_cast<int>(_mouse_down_window_region.size.height) - y_delta);
		style.auto_resize._height_auto_resize = false;
		return true;
	}

public:
	// Returns true if mouse hit on border, else returns false.
	bool TrackMsg(Msg msg, Para para) {
		if (!IsMouseMsg(msg)) { return false; }
		
		auto& mouse_msg = GetMouseMsg(para);
		BorderPosition border_position = PointToBorderPosition(
			_parent->GetSize(), _parent->GetStyle().border._width, mouse_msg.point
		);

		if (msg == Msg::MouseMove) {
			if (!_has_mouse_down) {
				// Mouse move only, change cursor according to border position.
				switch (border_position) {
				case BorderPosition::Left: SetCursor(Cursor::Resize0); break;
				case BorderPosition::Top: SetCursor(Cursor::Resize90); break;
				case BorderPosition::Right: SetCursor(Cursor::Resize0); break;
				case BorderPosition::Bottom: SetCursor(Cursor::Resize90); break;
				case BorderPosition::LeftTop: SetCursor(Cursor::Resize135); break;
				case BorderPosition::RightTop: SetCursor(Cursor::Resize45); break;
				case BorderPosition::LeftBottom: SetCursor(Cursor::Resize45); break;
				case BorderPosition::RightBottom: SetCursor(Cursor::Resize135); break;
				default: return false;
				}
			} else {
				Point current_position = _parent->ConvertToParentPoint(mouse_msg.point);
				Vector moving_vector = current_position - _mouse_down_position;

				// Mouse is dragging, resize the window, and clear auto resize style.
				auto& style = _parent->GetStyle();

				// Get the size of parent window to determine
				// !!! Doesn't work for ScrollWnd !!! But you could use GetEntireRegion().size instead.
				Size parent_size = _parent->GetParent()->GetSize();  // parent window can't be nullptr!

				bool has_position_changed = false;

				switch (_mouse_down_border_position) {
				case BorderPosition::Left:
					has_position_changed |= ResetLeftPosition(style, moving_vector.x, parent_size.width);
					break;
				case BorderPosition::Top:
					has_position_changed |= ResetTopPosition(style, moving_vector.y, parent_size.height);
					break;
				case BorderPosition::Right:
					has_position_changed |= ResetRightPosition(style, moving_vector.x);
					break;
				case BorderPosition::Bottom:
					has_position_changed |= ResetDownPosition(style, moving_vector.y);
					break;
				case BorderPosition::LeftTop:
					has_position_changed |= ResetLeftPosition(style, moving_vector.x, parent_size.width);
					has_position_changed |= ResetTopPosition(style, moving_vector.y, parent_size.height);
					break;
				case BorderPosition::RightTop:
					has_position_changed |= ResetTopPosition(style, moving_vector.y, parent_size.height);
					has_position_changed |= ResetRightPosition(style, moving_vector.x);
					break;
				case BorderPosition::LeftBottom:
					has_position_changed |= ResetLeftPosition(style, moving_vector.x, parent_size.width);
					has_position_changed |= ResetDownPosition(style, moving_vector.y);
					break;
				case BorderPosition::RightBottom:
					has_position_changed |= ResetRightPosition(style, moving_vector.x);
					has_position_changed |= ResetDownPosition(style, moving_vector.y);
					break;
				default: return false;
				}

				if (has_position_changed) {
					_parent->RegionStyleUpdated();
				}
			}
			return true;
		}

		else if (msg == Msg::LeftDown) {
			if (border_position == BorderPosition::None) { return false; }

			_has_mouse_down = true;
			_mouse_down_position = _parent->ConvertToParentPoint(mouse_msg.point);
			_mouse_down_border_position = border_position;
			_mouse_down_window_region = _parent->GetRegionOnParent();

			_parent->SetCapture();
			return true;
		}

		else if (msg == Msg::LeftUp) {
			if (_has_mouse_down) {
				_has_mouse_down = false;
				_parent->ReleaseCapture();
				return true;
			}
		}

		return false;
	}
};



END_NAMESPACE(WndDesignHelper)