#pragma once

#include "WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


// The synchronized timer bound with a window object. 
class Timer : Uncopyable{
private:
	ObjectBase& _wnd_object;
	Handle _timer;

private:
	static void TimerCallback(Handle hTimer, Ref<void*> object);

public:
	Timer(ObjectBase& wnd_object) : _wnd_object(wnd_object), _timer(nullptr) {}
	~Timer() { Stop(); }
	bool IsSet() const { return _timer != nullptr; }

	// Set a timer that will expire after delay(ms), and then for every period(ms).
	// The delay and period should be no less than 10ms, or it will not be as a
	// If the timer is already set, it will be reset.
	void Set(uint period);

	// Stop the timer.
	void Stop();

	// Called when the timer has expired. The timer will send a message to the window object directly.
	void Alert() { _wnd_object.Handler(nullptr, Msg::Timer, this); }
};


// The parameter of Msg::Timer is the pointer to the Timer.
inline Ref<Timer*> GetTimerMsg(Para para) {
	return reinterpret_cast<Ref<Timer*>>(para);
}


END_NAMESPACE(WndDesignHelper)