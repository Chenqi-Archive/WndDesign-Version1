#include "timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


void Timer::TimerCallback(Handle hTimer, Ref<void*> timer) {
	Timer* the_timer = static_cast<Timer*>(timer);
	if (the_timer->_timer == hTimer) { 
		the_timer->Alert();
	}
}

void Timer::Set(uint period, bool alert_immediately) {
	if (!IsSet()) {
		_timer = SetTimerSync(period, TimerCallback, this);
	} else {
		ResetTimerSync(_timer, period);
	}
	if (alert_immediately) {
		Alert();
	}
}

void Timer::Stop() {
	if (IsSet()) {
		KillTimerSync(_timer);
		_timer = nullptr;
	}
}


END_NAMESPACE(WndDesignHelper)