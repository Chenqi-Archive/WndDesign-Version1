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

	// Initialize desktop.
	IDesktop& desktop = IDesktop::Get();  

	// Define window style.
	TextBoxStyle style;
	style.size.normal(200px, 100px);
	style.position.setHorizontalCenter().setVerticalCenter();
	style.background.setColor(ColorSet::White);

	// Create a window.
	auto textbox = ITextBox::Create(&desktop, style, nullptr);
	// Or:
	// auto textbox = ITextBox::Create(nullptr, style, nullptr);
	// desktop.AddChild(textbox.get());

	// Set and update text.
	textbox->SetText(text);
	textbox->TextUpdated();

	// Enter message loop.
	auto ret = desktop.MessageLoop();

	return ret;
}


#endif // HelloWorld
