#include "timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


void Timer::TimerCallback(Handle hTimer, Ref<void*> timer) {
	static_cast<Timer*>(timer)->Alert();
}

void Timer::Set(uint period) {
	if (!IsSet()) {
		_timer = SetTimerSync(period, TimerCallback, this);
	} else {
		ResetTimerSync(_timer, period);
	}
}

void Timer::Stop() {
	if (IsSet()) {
		KillTimerSync(_timer);
		_timer = nullptr;
	}
}


END_NAMESPACE(WndDesignHelper)