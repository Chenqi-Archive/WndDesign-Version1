#define Init000
#ifdef Init000

#include "window/main_frame.h"


#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesignHelper.lib")
#endif


MainFrame* main_frame;

void Init(uint argc, const wchar_t* argv[]) {
	wstring file = L"1.cga";
	main_frame = new MainFrame(file);
}

void Final() {
	delete main_frame;  // main_frame will auto metically save file when destructs.
}

#endif
