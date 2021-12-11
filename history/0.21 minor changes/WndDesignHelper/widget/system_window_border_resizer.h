#pragma once

#include "../WndDesign.h"
#include "../system/aero_snap.h"
#include "../style/border_position.h"


BEGIN_NAMESPACE(WndDesignHelper)


class SystemWindowBorderResizer {
private:
	ObjectBase& _parent;

public:
	SystemWindowBorderResizer(ObjectBase& parent) : _parent(parent) {}
	bool TrackMsg(Msg msg, Para para) {
		if (!IsMouseMsg(msg)) {return false;}

		MouseMsg mouse_msg = GetMouseMsg(para);
		BorderPosition border_position = PointToBorderPosition(
			_parent->GetSize(), _parent->GetStyle().border._width, mouse_msg.point
		);

		if (msg == Msg::MouseMove) {
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
			return true;
		} else if (msg == Msg::LeftDown && border_position != BorderPosition::None) {
			SystemWindowBorderResizingLeftDown(_parent, mouse_msg.point, border_position);
			return true;
		}

		return false;
	}
};

END_NAMESPACE(WndDesignHelper)