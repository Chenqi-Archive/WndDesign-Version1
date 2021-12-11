
#include "window/main_frame.h"

#include "../WndDesignHelper/WndDesignHelper.h"

#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesignHelper.lib")
#endif

MainFrame* main_frame;

void Init() {
	main_frame = new MainFrame();
}

void Final() {
	delete main_frame;  // main_frame will auto metically save file when destructs.
}
