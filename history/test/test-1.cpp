#define SimpleTest

#ifdef SimpleTest

#include "../WndDesign/WndDesign.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")


using namespace WndDesign;

TextBox* text_box;

bool handler(WndBase* wnd, Msg msg, Para para) {
	if (!IsMouseMsg(msg)) { return false; }
	
	static bool has_mouse_down = false;
	static Point mouse_down_position;
	static bool has_moved = false;

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
		if (has_moved) { has_moved = false; }
		else { SendMsg(text_box, UserMsg(0), nullptr); }
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!has_mouse_down) {return true;}
		has_moved = true;
		Point current_position = mouse_msg.point;
		Vector moving_vector = current_position - mouse_down_position;
		Point origin_point = wnd->ConvertToParentPoint(point_zero);
		Point new_point = origin_point + moving_vector;
		wnd->SetPosition(px(new_point.x), px(new_point.y));
	}
	return true;
}

bool textbox_handler(WndBase* wnd, Msg msg, Para para) {
	TextBox* textbox = dynamic_cast<TextBox*>(wnd);
	if (textbox == nullptr) { return false; }

	static Timer timer;
	static const wchar text[] = L"😄IT之家8月22日消息表示";
	static const uint length = static_cast<uint>(wcslen(text));
	static uint number = length;

	if (msg == UserMsg(0)) {
		if (!timer.IsSet()) {
			timer.Set(wnd, 0, 500);
		}
		return true;
	}

	if (msg == Msg::Timer) {
		if (number > 2) {
			number--;
		} else {
			timer.Stop();
		}
		textbox->SetText(wstring(text, number));
		return true;
	}
	return false;
}

void Init() {
	WndStyle style;
	style.size.normal(800px, 480px);
	style.position.SetHorizontalCenter().SetVerticalCenter();
	style.background = ColorSet::GreenYellow;
	style.border.width(3px).color(ColorSet::Black);
	Wnd* main_wnd = Wnd::Create(desktop, style, nullptr);

	ButtonStyle floating_button_style;
	floating_button_style.size.SetFixed(50px, 50px);
	floating_button_style.position.left(10px).down(10px);
	floating_button_style.border.width(3px).color(ColorSet::Chocolate).radius(25);
	floating_button_style.render.position_change(true);
	floating_button_style.render.opacity(0x4f);
	Button* floating_button = Button::Create(main_wnd, floating_button_style, handler);

	TextBoxStyle textbox_style;
	textbox_style.size.normal(200px, 200px);
	textbox_style.position.SetHorizontalCenter().SetVerticalCenter();
	textbox_style.background = color_transparent;
	textbox_style.border.color(ColorSet::DarkGreen).width(10px);
	textbox_style.padding.SetAll(20px);
	textbox_style.font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
	text_box = TextBox::Create(main_wnd, textbox_style, L"", textbox_handler);
}

void Final() {

}

#endif