#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


using HANDLE = void*;
using TimerCallback = void (*)(void* para);

// Timer messages will be sent synchronously to the main thread's message queue.
// If timer == nullptr, a new timer is created, or the old timer will be reset.
// The period should be at least 10(ms).
WNDDESIGN_API HANDLE SetTimerSync(uint period, TimerCallback callback, void* para);
WNDDESIGN_API bool ResetTimerSync(HANDLE timer, uint period);
WNDDESIGN_API bool KillTimerSync(HANDLE timer);


//HANDLE SetTimerAsync(HANDLE timer, uint delay, uint period, TimerCallback callback, void* para);
//void KillTimerAsync(HANDLE timer);


END_NAMESPACE(WndDesign)