#include "common/core.h"
#include "window/desktop_impl.h"

#include <Windows.h>

using WndDesign::_desktop;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR szCmdline, int iCmdShow)
{
	void (*Init)();
	void (*Finish)();

	Init = (void(*)())GetProcAddress(hInstance, "Init");
	Finish = (void(*)())GetProcAddress(hInstance, "Finish");

	if (!Init) {
		MessageBox(NULL, L"Could not find function \"Init\"", L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	Init();

	_desktop->Render();
	_desktop->EnterMessageLoop();

	if (!Finish) {
		return 0;
	}

	Finish();
	return 0;
}