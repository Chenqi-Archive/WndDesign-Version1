#define _CRT_SECURE_NO_WARNINGS

#include "debug_helper.h"

#include <cstdio>

#undef Alloc
#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)

void OpenConsole() {
    AllocConsole();
    freopen("CON", "r", stdin);
    freopen("CON", "w", stdout);
}

void CloseConsole() {
    FreeConsole();
}

END_NAMESPACE(WndDesign)
