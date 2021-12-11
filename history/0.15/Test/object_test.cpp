#define ObjectTest

#ifdef ObjectTest

#include "../WndDesignHelper/WndDesignHelper.h"

#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")

using namespace WndDesignHelper;

class MainFrame;

const wchar text[] = L"😄IT之家8月22日消息表示";

class MyTextBox : public TextBox {
private:
	MainFrame& _parent;
	bool has_mouse_down = false;
	Point mouse_down_position = point_zero;

public:
	MyTextBox(MainFrame& parent) :_parent(parent) {}
	void Init();
	bool Handler(IWndBase* wnd, Msg msg, Para para) override;
};

class MainFrame : public SimpleScrollWnd {
private:
	bool has_mouse_down = false;
	Point mouse_down_position = point_zero;

	MyTextBox _textbox;

public:
	MainFrame() :_textbox(*this){}
	void Init() {
		ScrollWndStyle style;
		style.size.normal(800px, 480px);
		style.position.SetHorizontalCenter().SetVerticalCenter();
		style.background = ColorSet::White;
		style.border.width(3px).color(ColorSet::Black);
		style.entire_region.Set(100pct, 200pct); // = scroll_region_infinite;
		SimpleScrollWnd::Init(&desktop, style);

		_textbox.Init();
	}
	void Destroy() {
		SimpleScrollWnd::Destroy();
	}
	bool Handler(IWndBase* wnd, Msg msg, Para para) {
		if (SimpleScrollWnd::Handler(wnd, msg, para)) {
			return true;
		}

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
}main_frame;


void MyTextBox::Init() {
	TextBoxStyle textbox_style;
	textbox_style.size.normal(200px, 200px);
	textbox_style.position.SetHorizontalCenter().SetVerticalCenter();
	textbox_style.background = color_transparent;
	textbox_style.border.color(ColorSet::DarkGreen).width(10px);
	textbox_style.padding.SetAll(20px);
	textbox_style.font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
	TextBox::Init(nullptr, textbox_style);
	(*this)->SetText(text);
	_parent->AddChildStatic(GetWnd());
}

bool MyTextBox::Handler(IWndBase* wnd, Msg msg, Para para) {
	if (GetWnd() != wnd) { return false; }
	if (!IsMouseMsg(msg)) { return false; }

	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		has_mouse_down = true;
		mouse_down_position = mouse_msg.point;
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

void Init() {
	main_frame.Init();
}

void Final() {
	main_frame.Destroy();
}


#endif
