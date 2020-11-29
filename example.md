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

    // Set and update text.
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

  * 初始化desktop。

  - 定义文本框样式：大小为200px*100px，位置水平和垂直居中，背景为白色。
  - 创建文本框，指定父窗口和样式，最后一个参数暂时填nullptr。
  - 设置文本框内的文字并刷新
  - desktop进入消息循环




注：

- 窗口按照层级结构组织，Desktop（桌面）为根父窗口，地位相当于Window下的Desktop Window     Manager。

- 窗口均为接口类，通过Create静态函数创建，创建的窗口由unique_ptr封装，使用纯虚函数接口访问。创建窗口时可以指定父窗口，或者稍后通过调用父窗口的AddChild添加为子窗口。因此样例程序也可写作：

  ```c++
  auto textbox = ITextBox::Create(nullptr, style, nullptr);
  desktop.AddChild(textbox); // 此时该窗口将被显示
  ```

- 目前有以下5种窗口类型（按继承关系树状组织）：
  * WndBase：所有窗口接口的基类，体现为空白窗口，单纯显示背景和边框，不能添加子窗口。
    * TextBox：文本框窗口，可以关联某个wstring文本，可以动态调整大小。
    * Wnd：可以添加子窗口。

      * ScrollWnd：可以添加子窗口，底层窗口可滚动。
    * Table：表格。
    * Desktop：桌面（非具体窗口），用于提供系统接口。






### 借助WndDesignHelper更方便地构建GUI应用

WndDesignHelper使用类对各种窗口接口进行了进一步封装，并提供了更多窗口控件和工具，帮助您构建复杂的应用程序。

使用WndDesignHelper构建HelloWorld：

```c++
// Test/helloworld-object.cpp

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
- 定义MyTextbox类，继承自TextBox类，并定义样式及构造函数，使用desktop作为父窗口，在构造函数内设置并更新文本。
- 定义Init()及Final()函数，在Init()内创建MyEditBox。

 

注：

- WndDesignHelper内已经定义了WinMain函数，并已经初始化了desktop，并会调用Init()函数实现用户程序的初始化。您只需要定义Init函数及Final函数进行初始化和清理工作。
- TextBox类中维护了ITextBox指针，会在析构时自动释放对象。
- 使用WndDesignHelper，您可以处理窗口消息，只需重载虚函数Handler即可。
- WndDesignHelper对WndDesign中的接口类进行了对象化的封装，除WndBase，Wnd，ScrollWnd，Table，TextBox之外，还在其基础上添加了EditBox，Button等类型，您可以以此为参考，自己定义更多的窗口类型。





更详细的接口说明，可以直接参考代码及注释。

