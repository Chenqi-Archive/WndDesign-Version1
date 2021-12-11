#define SimpleTest

#ifdef SimpleTest

#include "../WndDesign/WndDesign.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")


using namespace WndDesign;

//SimpleScrollBar scroll_bar;

bool textbox_handler(WndBase* wnd, Msg msg, Para para) {
	TextBox* textbox = dynamic_cast<TextBox*>(wnd);
	if (textbox == nullptr) { return false; }
	static const wchar text[] = L"😄IT之家8月22日消息表示";
	if (msg == Msg::Create) {
		textbox->SetText(text);
		return true;
	}
	return false;
}

bool scrollwnd_handler(WndBase* wnd, Msg msg, Para para) {
	if (!IsMouseMsg(msg)) { return false; }

	auto scrollwnd = dynamic_cast<ScrollWnd*>(wnd);
	
	static bool has_mouse_down = false;
	static Point mouse_down_position;

	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		mouse_down_position = mouse_msg.point;
		has_mouse_down = true;
		wnd->SetCapture();
		return true;
	}
	if (msg == Msg::LeftUp) {
		has_mouse_down = false;
		wnd->ReleaseCapture();
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!has_mouse_down) { return true; }
		Point current_position = mouse_msg.point;
		Vector moving_vector = mouse_down_position - current_position ;
		scrollwnd->MoveView(moving_vector);
		mouse_down_position = current_position;
		return true;
	}
	return true;
}

void Init() {
	ScrollWndStyle style;
	style.size.normal(800px, 480px);
	style.position.SetHorizontalCenter().SetVerticalCenter();
	style.background = ColorSet::GreenYellow;
	style.border.width(3px).color(ColorSet::Black);
	style.entire_region.Set(100pct, 200pct); // = scroll_region_infinite;
	//style.scroll_bar = &scroll_bar;
	auto main_wnd = ScrollWnd::Create(desktop, style, scrollwnd_handler);

	TextBoxStyle textbox_style;
	textbox_style.size.normal(200px, 200px);
	textbox_style.position.left(0px).top(0px);
	textbox_style.background = color_transparent;
	textbox_style.border.color(ColorSet::DarkGreen).width(10px);
	textbox_style.padding.SetAll(20px);
	textbox_style.font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
	auto text_box = TextBox::Create(nullptr, textbox_style, L"", textbox_handler);
	main_wnd->AddChildStatic(text_box);
}

void Final() {

}

#endif