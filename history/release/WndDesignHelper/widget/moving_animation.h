#pragma once

#include "../WndDesign.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


// It takes the input of large values, break it up to to smalls values and output
//   on a regular basis using a timer.
// Used for smooth moving animations like scrolling.

// May support kinetic scrolling

class MovingAnimation {
private:
	static const int period = 10;  // ms
	static const int time_to_finish = 100;  // ms
	static const int min_delta = 12;
	Timer _timer;
	int _delta = 0;
	int _remaining_amount = 0;

public:
	MovingAnimation(ObjectBase& wnd_object) :
		_timer(wnd_object) {
	}

	// Add more amount to the remainint amount.
	void AddUp(int new_amount) {
		if (!_timer.IsSet()) { _timer.Set(period); }
		_remaining_amount += new_amount;
		_delta = _remaining_amount * static_cast<int>(period) / static_cast<int>(time_to_finish);
		if (_delta > 0) {
			_delta = max(_delta, min_delta);
		} else {
			_delta = min(_delta, -min_delta);
		}
	}

	// Track timer messages to output small amount per time.
	int TrackTimerMsg(Msg msg, Para para) {
		if (msg != Msg::Timer || GetTimerMsg(para) != &_timer) { return 0; }
		int delta;
		if (_remaining_amount > 0) {
			delta = min(_remaining_amount, _delta);
		} else {
			delta = max(_remaining_amount, _delta);
		}
		_remaining_amount -= delta;
		if (_remaining_amount == 0) { _timer.Stop(); }
		return delta;
	}
};



END_NAMESPACE(WndDesignHelper)