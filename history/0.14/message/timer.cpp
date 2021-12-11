#include "timer.h"
#include "../win32/win32_api.h"
#include "../window/wnd_base_impl.h"

#include <unordered_map>

BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)

using std::unordered_map;

class TimerMap: private unordered_map<HANDLE, Ref Timer*> {
private:
	TimerMap() {}
	friend TimerMap& GetMap();
public:
	void Insert(HANDLE timer, Ref Timer* object) {
		insert(std::make_pair(timer, object));
	}
	void Erase(HANDLE timer) {
		erase(timer);
	}
	Ref Timer* Find(HANDLE timer) {
		auto it = find(timer);
		if (it == end()) { return nullptr; }
		return it->second;
	}
};

TimerMap& GetMap() {
	static TimerMap timer_map;
	return timer_map;
}

END_NAMESPACE(Anonymous)


void SystemTimerHandler(HANDLE timer) {
	Ref Timer* object = GetMap().Find(timer);
	if (object == nullptr) {
		Win32::KillTimer(timer);
	} else {
		object->Alert();
	}
}


void Timer::Set(WndBase* wnd, uint delay, uint period, TimerHandler handler) {
	_wnd = wnd;
	_handler = _handler;
	_period = period;
	if (delay == 0) {
		// Alert immediately.
		_has_delayed = true;
		Alert();
		if (period != 0) {
			_timer = Win32::SetTimer(_timer, period);
			GetMap().Insert(_timer, this);  // The timer will not be changed.
		}
	} else {
		_has_delayed = false;
		_timer = Win32::SetTimer(_timer, delay);
		GetMap().Insert(_timer, this);
	}
}

void Timer::Stop() {
	if (_timer != nullptr) {
		Win32::KillTimer(_timer);
		GetMap().Erase(_timer);
		_timer = nullptr;
	}
}

void Timer::Alert() {
	if (_has_delayed == false) {
		_has_delayed = true;
		if (_period != 0) {
			Win32::SetTimer(_timer, _period);
		} else {
			Stop();
		}
	}
	if (_handler == nullptr) {
		dynamic_cast<_WndBase_Impl*>(_wnd)->DispatchMessage(Msg::Timer, this);
	} else {
		_handler(_wnd, Msg::Timer, this);
	}
}


END_NAMESPACE(WndDesign)