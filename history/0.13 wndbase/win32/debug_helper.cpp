#include "debug_helper.h"

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

#undef Alloc
#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)

void WNDDESIGN_API OpenConsole() {
    AllocConsole();
    freopen("CON", "r", stdin);
    freopen("CON", "w", stdout);
}

void WNDDESIGN_API CloseConsole() {
    FreeConsole();
}

END_NAMESPACE(WndDesign)
