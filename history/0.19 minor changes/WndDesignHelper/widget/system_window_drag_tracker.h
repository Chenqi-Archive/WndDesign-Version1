#pragma once

#include "../WndDesign.h"
#include "../system/aero_snap.h"

BEGIN_NAMESPACE(WndDesignHelper)


// The window drag tarcker that allows aero snapping on Win32.
// Note that when the tracker has handled the LeftDown message, the subsequent MouseMove and LeftUp will not be received.
class SystemWndDragTracker {
private:
	ObjectBase& _parent;

public:
	SystemWndDragTracker(ObjectBase& parent) : _parent(parent){}
	void TrackMsg(Msg msg, Para para) {
		if (msg == Msg::LeftDown) {
			MouseMsg mouse_msg = GetMouseMsg(para);
			SystemWindowDraggingLeftDown(_parent, mouse_msg.point);
		} 
		// After sending the nonclient left down message, all related messages will not be processed.
		//else if (msg == Msg::MouseMove) {
		//	SystemWindowDraggingMouseMove(_parent, mouse_msg.point);
		//} else if (msg == Msg::LeftUp) {
		//	SystemWindowDraggingLeftUp(_parent, mouse_msg.point);
		//}
	}
};


END_NAMESPACE(WndDesignHelper)