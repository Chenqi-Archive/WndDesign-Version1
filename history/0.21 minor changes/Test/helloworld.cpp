// #define HelloWorld

#ifdef HelloWorld

#include "../WndDesign/WndDesign.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")

using namespace WndDesign;


int main() {
	IDesktop& desktop = IDesktop::Get();  // Initialize desktop.

	TextBoxStyle style;
	style.size.normal(200px, 100px);
	style.position.setHorizontalCenter().setVerticalCenter();
	auto textbox = ITextBox::Create(&desktop, style, nullptr);
	textbox->SetText(L"Hello World!");

	auto ret = desktop.MessageLoop();

	return ret;
}


#endif // HelloWorld
