#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


using Handle = void*;
using TimerCallback = void (*)(Handle timer, void* para);

// Timer messages will be sent synchronously to the main thread's message queue.
// If timer == nullptr, a new timer is created, or the old timer will be reset.
// The period should be at least 10(ms).
WNDDESIGN_API Handle SetTimerSync(uint period, TimerCallback callback, void* para);
WNDDESIGN_API bool ResetTimerSync(Handle timer, uint period);
WNDDESIGN_API bool KillTimerSync(Handle timer);


//Handle SetTimerAsync(Handle timer, uint delay, uint period, TimerCallback callback, void* para);
//void KillTimerAsync(Handle timer);


END_NAMESPACE(WndDesign)