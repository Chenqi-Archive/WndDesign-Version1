# WndDesign图形界面库

An effective and friendly c++ Win32 gui library based on Direct2D and DirectWrite.

纯c++编写的Win32图形界面库，底层依赖于Direct2D和DirectWrite。



使用Visual Studio 2019编译（c++17）



### 快速熟悉WndDesign

使用WndDesign构建HelloWorld：

```c++
// Test/helloworld.cpp

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

	textbox->SetText(text);
	textbox->TextUpdated();

    // Enter message loop.
	auto ret = desktop.MessageLoop();

	return ret;
}

```



程序说明：

- 引用WndDesign头文件及动态链接库对应的lib文件。

- 使用命名空间WndDesign。

- 在main函数（或者WinMain函数）中：

- - 获得desktop（desktop作为直接系统窗口的父窗口），同时desktop将被初始化。
  - 定义文本框样式：大小为200px*100px，位置水平和垂直居中。
  - 创建文本框，指定父窗口和样式，最后一个参数暂时填nullptr。
  - 设置文本框内的文字
  - desktop进入消息循环

 

注：

- Desktop（桌面，有时也称作System）为根父窗口，地位相当于Window下的Desktop Window     Manager。
- 所有窗口均按父子窗口关系呈树状组织，创建窗口时可以不指定父窗口（该窗口也不会显示），之后可以通过父窗口添加或去除一个子窗口。因此样例程序也可写作：

auto textbox = ITextBox::Create(nullptr, style, nullptr);

textbox->SetText(L"Hello World!");

desktop.AddChild(textbox); // 此时该窗口将被显示

- 窗口创建后，使用纯虚函数提供的接口访问对象。

- 目前有以下5种窗口类型（按继承关系树状组织）：

- - WndBase：所有窗口接口的基类，体现为空白窗口，不能添加子窗口。

  - - TextBox：文本框窗口。

    - Wnd：可以添加子窗口。

    - - ScrollWnd：可以添加子窗口，底层窗口可滚动。

    - Table：表格。

    - Desktop：（桌面，非具体窗口）。

- 更多详细的使用帮助可见注释。（文档有时间会补充）



### 借助WndDesignHelper更方便地构建GUI应用

WndDesignHelper使用类对各种窗口接口进行了进一步封装，并提供了更多窗口控件和工具，帮助您构建复杂的应用程序。

使用WndDesignHelper构建HelloWorld：

```c++
#include "../WndDesignHelper/WndDesignHelper.h"

#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesignHelper.lib")
#endif

using namespace WndDesignHelper;

class MyTextbox : public TextBox {
	struct Style : public TextBoxStyle {
		Style() {
			size.normal(200px, 100px);
			position.setHorizontalCenter().setVerticalCenter();
			background.setColor(ColorSet::White);
		}
	};

	wstring text = L"Hello World!";

public:
	MyTextbox() : TextBox(desktop, Style(), text) {
		(*this)->TextUpdated();
	}

	bool Handler(Msg msg, Para para) override {
		if (msg == Msg::LeftUp) {
			desktop->Finish();
			// Another way to exit: 
			// desktop->RemoveChild(GetWnd());
		}
		return true;
	}
};
MyTextbox* textbox;

void Init(uint argc, const wchar_t* argv[]) {
	textbox = new MyTextbox;
}

void Final() {
	delete textbox;
}
```



程序说明：

- 引用WndDesignHelper头文件，命名空间及静态库（此静态库已经包含WndDesign动态库的接口，无需再次引用WndDesign.lib）
- 定义文本编辑框派生自文本编辑框控件，并定义样式及构造函数，使用desktop作为父窗口，并在构造函数内设置文本。
- 定义Init()及Final()函数，在Init()内创建MyEditBox()。

 

注：

- WndDesignHelper内已经定义了WinMain函数，并已经初始化了desktop，并会调用Init()函数实现用户程序的初始化。