#include "../common/core.h"
#include "../window/desktop_impl.h"

#undef Alloc
#include <Windows.h>

using namespace WndDesign;

HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR szCmdline, int iCmdShow) {
	::hInstance = hInstance;

	desktop = _Desktop_Impl::Get();  // Initialize desktop.

	void (*Init)();
	void (*Final)();

	Init = (void(*)())GetProcAddress(hInstance, "Init");
	Final = (void(*)())GetProcAddress(hInstance, "Final");

	if (Init != nullptr) {
		Init();
	} else {
		MessageBox(NULL, L"Could not find function \"Init\"", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	_Desktop_Impl::Get()->EnterMessageLoop();

	if (Final != nullptr) {
		Final();
	}

	return 0;
}