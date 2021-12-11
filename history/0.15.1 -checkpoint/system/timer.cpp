#include "timer.h"

#include <Windows.h>

#include <unordered_map>


BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)


using TimerCallbackStruct = struct { TimerCallback callback; void* para; };

std::unordered_map<Handle, TimerCallbackStruct> timer_sync_map;

// The default synchornized timer callback.
void TimerCallbackSync(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4) {
    auto it = timer_sync_map.find(reinterpret_cast<Handle>(Arg3));
    if (it == timer_sync_map.end()) {  // The timer may have been killed, ignore the message.
        return; 
    } 
    it->second.callback(it->first, it->second.para);
}


END_NAMESPACE(Anonymous)


WNDDESIGN_API Handle SetTimerSync(uint period, TimerCallback callback, void* para) {
    Handle timer = reinterpret_cast<Handle>(SetTimer(NULL, NULL, period, TimerCallbackSync));
    timer_sync_map.emplace(timer, TimerCallbackStruct{ callback, para });
    return timer;
}

WNDDESIGN_API bool ResetTimerSync(Handle timer, uint period) {
    if (timer_sync_map.find(timer) == timer_sync_map.end()) { return false; }
    SetTimer(NULL, reinterpret_cast<UINT_PTR>(timer), period, TimerCallbackSync);
    return true;
}

WNDDESIGN_API bool KillTimerSync(Handle timer) {
    auto it = timer_sync_map.find(timer);
    if (it == timer_sync_map.end()) { return false; }
    KillTimer(NULL, reinterpret_cast<UINT_PTR>(timer));
    timer_sync_map.erase(it);
    return true;
}


//Handle SetTimerAsync(Handle timer, uint delay, uint period, TimerCallback callback, void* para) {
//    if (timer == NULL) {
//        CreateTimerQueueTimer(&timer, NULL, callback, callback, delay, period, WT_EXECUTEDEFAULT);
//    } else {
//        ChangeTimerQueueTimer(NULL, timer, delay, period);
//    }
//    return timer;
//}
//
//void KillTimerAsync(Handle timer) {
//    DeleteTimerQueueTimer(NULL, timer, INVALID_HANDLE_VALUE);
//}

END_NAMESPACE(WndDesign)