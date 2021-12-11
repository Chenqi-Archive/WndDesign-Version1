#include "../WndDesign/WndDesign.h"
#include "../WndDesign/Helper.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")

using namespace WndDesign;

Alloc Wnd* main_wnd;
WndStyle main_style;
SolidColor background_color;

Alloc Button* close_button;
ButtonStyle close_button_style;

Alloc ScrollWnd* scroll_wnd;
WndStyle scroll_style;
Bitmap scroll_bitmap;

Alloc TextBox* textbox;
TextBoxStyle textbox_style;
wstring text = L"InfiniteTarget维护直接子窗口的空间索引（仅限于MultiLayer），"
"可见区域更新时，相关的子窗口重画。\n子窗口会重新合成，但如果遇到拥有InfiniteTarget的layer，"
"会先计算可见区域，对这个layer进行重画，再做后续的合成。";

Alloc Table* table;
TableStyle table_style;


bool main_handler(Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		main_style.opacity ^= 0x80;
		// Need to call setstyle to take immediate effect, or will wait until the next render.
		main_wnd->SetStyle(&main_style);   // Will check the computed main_style.
	}
	return true;
}

bool close_button_handler(Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		// Messagebox
		DestroyWnd(main_wnd); // All sub windows will be destroyed.
		main_wnd = nullptr;
		close_button = nullptr;
		scroll_wnd = nullptr;
		textbox = nullptr;
	}
}

void Init() {
	// Main window
	main_style.size.normal = { 900px, 600px };
	main_style.size.min = { 100px, 100px };
	main_style.size.max = infinite_size;
	main_style.margin.left = 100;
	main_style.margin.top = 100;
	main_style.border.width = 5px;
	main_style.border.color = ColorSet::Green;
	main_style.background = &(background_color = ColorSet::GreenYellow);  // Background is a pointer to texture.
	main_wnd = Wnd::Create(desktop, &main_style, main_handler);

	// Close button
	close_button_style.size = { 50px, 50px };  // size.normal
	close_button_style.margin.right = 20px;
	close_button_style.margin.top = 20px;
	close_button_style.border.width = 3px;
	close_button_style.border.color = ColorSet::DodgerBlue;
	//close_button_style.background;
	close_button = Button::Create(main_wnd, &close_button_style, close_button_handler);
	close_button->SetText(L"X");

	// ScrollWnd
	scroll_style.size.normal = { 500px, 500px };
	scroll_style.margin.left = 50px;
	scroll_style.margin.top = 50px;
	scroll_style.border.Set(3px, ColorSet::DarkGreen);
	scroll_style.background = &scroll_bitmap; scroll_bitmap.Load(L"/resource/1.jpg");
	scroll_style.opacity = 0xF0;
	scroll_wnd = ScrollWnd::Create(main_wnd, &scroll_style, nullptr);

	// TextBox
	textbox_style.size.normal = { 600px, 700px };
	textbox_style.size.max = { 100px, 50px };
	textbox_style.background = &background_color;  // Share the same color with the main window's background.
	textbox = TextBox::Create(nullptr, &textbox_style, nullptr); scroll_wnd->AddChild(textbox);
	textbox->SetText(text);
}

void Finish() {
	//main_wnd->Destroy(); // Optional
}