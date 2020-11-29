#define HelloWorld_Object

#ifdef HelloWorld_Object

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

#endif