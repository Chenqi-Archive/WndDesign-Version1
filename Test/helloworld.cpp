//#define HelloWorld

#ifdef HelloWorld

#include "../WndDesign/WndDesign.h"

#include <Windows.h>


#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesign.lib")
#endif


using namespace WndDesign;

wstring text = L"Hello World!";


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdline, int iCmdShow) {

	IDesktop& desktop = IDesktop::Get();  // Initialize desktop.

	TextBoxStyle style;
	style.size.normal(200px, 100px);
	style.position.setHorizontalCenter().setVerticalCenter();
	style.background.setColor(ColorSet::White);
	auto textbox = ITextBox::Create(&desktop, style, nullptr);

	textbox->SetText(text);
	textbox->TextUpdated();

	auto ret = desktop.MessageLoop();

	return ret;
}


#endif // HelloWorld
