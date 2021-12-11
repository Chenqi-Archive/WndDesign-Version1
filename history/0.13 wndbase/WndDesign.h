#pragma once

#include "window/window.h"
#include "style/style.h"

BEGIN_NAMESPACE(WndDesign)

template<class WndType>
inline void SafeDestroy(WndType** pWnd) {
	// if (*pWnd == nullptr) { return; }
	*pWnd->Destroy();
	*pWnd = nullptr;
}

//template<class WndType>
//inline void SafeDestroy(WndType*& pWnd) {
//	// if (*pWnd == nullptr) { return; }
//	DestroyWnd(pWnd);
//	pWnd = nullptr;
//}


END_NAMESPACE(WndDesign)


#define ___   // A placeholder

extern "C" __declspec(dllexport) void Init();
extern "C" __declspec(dllexport) void Final();