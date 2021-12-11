#pragma once

#include "../WndDesign.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)

// Auto fade the window when mouse leaves, and show it when mouse hovers.
class FadingAnimation {
private:
	inline static const uint _period = 40;  // 40ms
	inline static const uint _time_to_finish = 2560; // 2560ms = 40ms * 64times
	ObjectBase& _parent;
	Timer _timer;
	uchar _current_opacity;

public:
	FadingAnimation(ObjectBase& parent) :
		_parent(parent), _timer(parent), _current_opacity(_parent->GetStyle().render._opacity) {
	}

	void TrackMsg(Msg msg, Para para) {
		if (IsMouseMsg(msg)) {
			_timer.Stop();
			if (_current_opacity != 0xFF) {
				_current_opacity = 0xFF;
				_parent->SetOpacity(_current_opacity);
			}
		}
		if (msg == Msg::MouseLeave) {
			_timer.Set(_period);
		}
		if (msg == Msg::Timer && GetTimerMsg(para) == &_timer) {
			uchar delta = static_cast<uchar>(0xFF * _period / _time_to_finish);  // 4
			if (_current_opacity > delta) {
				_current_opacity -= delta;
			} else {
				_current_opacity = 0;
				_timer.Stop();
			}
			_parent->SetOpacity(_current_opacity);
		}
	}
};



END_NAMESPACE(WndDesignHelper)