#include "../object/WndTypeBase.h"

//#pragma comment(lib, "../../build/x64/Debug/WndDesign.lib")
#pragma comment(lib, "C:/Users/10026/source/WndDesign/build/x64/Debug/WndDesign.lib")

#include <Windows.h>

BEGIN_NAMESPACE(WndDesignHelper)
Desktop desktop;
END_NAMESPACE(WndDesignHelper)

using namespace WndDesignHelper;


extern void Init();
extern void Final();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR szCmdline, int iCmdShow) {
	desktop.Init();
	Init();
	int ret = desktop->MessageLoop();
	Final();
	return ret;
}