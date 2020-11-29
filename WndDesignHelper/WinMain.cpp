#include "object/WndTypeBase.h"


#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesign.lib")
#endif


#include <Windows.h>

BEGIN_NAMESPACE(WndDesignHelper)
Desktop desktop;
END_NAMESPACE(WndDesignHelper)

using namespace WndDesignHelper;


extern void Init(uint argc, const wchar_t* argv[]);
extern void Final();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR szCmdline, int iCmdShow) {

	const wchar* cmdline = GetCommandLineW();
	int argc;
	const wchar_t** argv = (const wchar_t**)(CommandLineToArgvW(cmdline, &argc));

	Init(static_cast<uint>(argc), argv);

	int ret = desktop->MessageLoop();

	Final();

	return ret;
}