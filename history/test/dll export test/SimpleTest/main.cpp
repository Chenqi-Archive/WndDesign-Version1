
#include "../DllTest/header.h"

#ifdef _DEBUG
#pragma comment(lib, "../build/x64/debug/DllTest.lib")
#else
#pragma comment(lib, "../build/x64/release/DllTest.lib")
#endif


int main() {
	B b;
	A* a = &b;
	a->f();
}