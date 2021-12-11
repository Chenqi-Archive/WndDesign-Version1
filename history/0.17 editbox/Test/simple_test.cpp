//#define SimpleTest

#ifdef SimpleTest

#include "../WndDesign/WndDesign.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")

#include <Windows.h>

#undef max

using namespace WndDesign;
using namespace WndDesignHelper;


class TextBox :public ObjectBase {
private:
	int click = 1;
	bool has_mouse_down = false;
	Point mouse_down_position;

public:
	bool Handler(IWndBase* wnd, Msg msg, Para para) override {
		ITextBox* textbox = dynamic_cast<ITextBox*>(wnd);
		if (textbox == nullptr) { return false; }
		static const wchar text[] = L"😄IT之家8月22日消息表示";
		if (msg == Msg::Create) {
			textbox->SetText(text);
			return true;
		}
		if (!IsMouseMsg(msg)) { return false; }

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
			Vector moving_vector = current_position - mouse_down_position;
			if (moving_vector == vector_zero) { return true; }
			Point origin_point = wnd->ConvertToLayerPoint(point_zero);
			Point new_point = origin_point + moving_vector;
			wnd->SetPosition(px(new_point.x), px(new_point.y));
			return true;
		}
		if (msg == Msg::RightUp) {
			IDesktop::Get()->Destroy();
			return true;
		}

		return false;
	}
}textbox_object;

class ScrollWnd : public ObjectBase {
private:
	bool has_mouse_down = false;
	Point mouse_down_position;
public:
	bool Handler(IWndBase* wnd, Msg msg, Para para) {
		if (!IsMouseMsg(msg)) { return false; }

		auto scrollwnd = dynamic_cast<IScrollWnd*>(wnd);

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
			Vector moving_vector = mouse_down_position - current_position;
			scrollwnd->MoveView(moving_vector);
			mouse_down_position = current_position;
			return true;
		}
		if (msg == Msg::RightUp) {
			IDesktop::Get()->Destroy();
			return true;
		}
		return true;
	}
}scrollwnd_object;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR szCmdline, int iCmdShow) {
	IDesktop* desktop = IDesktop::Get();  // Initialize desktop.

	ScrollWndStyle style;
	style.size.normal(800px, 480px);
	style.position.setHorizontalCenter().setVerticalCenter();
	style.background.setColor(ColorSet::White);
	style.border.width(3px).color(ColorSet::Black);
	style.entire_region.Set(0pct, 0pct, 100pct, 100pct); // = scroll_region_infinite;
	auto mainwnd = IScrollWnd::Create(desktop, style, &scrollwnd_object);
	
	TextBoxStyle textbox_style;
	textbox_style.size.max(200px, 200px);
	textbox_style.auto_resize.set(true, true);
	textbox_style.position.setHorizontalCenter().setVerticalCenter();
	textbox_style.background.setColor(color_transparent);
	textbox_style.border.color(ColorSet::DarkGreen).width(10px);
	textbox_style.padding.SetAll(20px);
	textbox_style.font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
	auto textbox = ITextBox::Create(nullptr, textbox_style, &textbox_object);
	mainwnd->AddChildStatic(textbox);

	desktop->MessageLoop();

	textbox->Destroy();
	mainwnd->Destroy();

	return 0;
}

#endif