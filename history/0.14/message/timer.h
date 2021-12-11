#pragma once

#include "msg.h"

BEGIN_NAMESPACE(WndDesign)

using TimerHandler = Handler;
using HANDLE = void*;

// The timer bound with a window. 
// Timer messages will be sent synchronously to the main thread's message queue.
class WNDDESIGN_API Timer {
private:
	HANDLE _timer;
	WndBase* _wnd;
	TimerHandler _handler;
	uint _period;
	bool _has_delayed;

public:
	Timer():_timer(nullptr), _wnd(nullptr), _handler(nullptr),_period(0) ,_has_delayed(false){}
	~Timer() { Stop(); }

	// Uncopyable.
	Timer(const Timer& timer) = delete;
	Timer& operator=(const Timer& timer) = delete;

	bool IsSet() const { return _timer != nullptr; }

	// Set a timer with a specified period(ms), the first signal will be sent after the delay(ms).
	// If period is 0ms, the timer will only be called once. Delay and period should be bigger than 10ms.
	// If the timer is already set, it will be reset.
	void Set(WndBase* wnd, uint delay, uint period = 0, TimerHandler handler = nullptr);

	// Stop the timer.
	void Stop();

	// Called by system to indicate a timer message.
	void Alert();
};


// The parameter of Msg::Timer is the pointer to the Timer.
inline Ref Timer* GetTimerMsg(Para para) {
	return reinterpret_cast<Timer*>(para);
}


END_NAMESPACE(WndDesign)