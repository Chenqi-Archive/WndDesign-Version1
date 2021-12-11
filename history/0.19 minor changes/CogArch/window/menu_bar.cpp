#include "menu_bar.h"
#include "main_frame.h"


inline MenuBar::Style::Style() {
	size.setFixed(100pct, 30px);
	position.left(0px).top(0px);
	background.setColor(ColorSet::DarkSlateBlue);
}

inline MenuBar::Title::Style::Style() {
	size.normal(length_auto, 100pct).max(100pct, 100pct);
	auto_resize.width_auto_resize();
	position.down(0px).setHorizontalCenter();
	background.setColor(color_transparent);
	padding.setAll(7px);
	paragraph.text_align(TextAlign::Leading).word_wrap(WordWrap::NoWrap);
	font.size(16px).color(ColorSet::White);
}

MenuBar::CloseButton::Style::Style() {
	size.setFixed(40px, 30px);
	position.right(0px).top(0px);
	background.setColor(color_transparent);
	background_hover.setColor(Color(ColorSet::DarkGray, 0x7F));
	background_down.setColor(Color(ColorSet::Gray, 0x7F));
}

inline MenuBar::Menu::Style::Style(uint start_position) {
	size.normal(50px, 20px);
	position.left(px(start_position)).setVerticalCenter();
	background.setColor(ColorSet::LightSalmon);
	border.radius(10px);
	padding.setAll(2px);
	paragraph.text_align(TextAlign::Center).paragraph_align(ParagraphAlign::Center).word_wrap(WordWrap::NoWrap);
	font.size(16px).color(ColorSet::White);

	background_hover.setColor(ColorSet::Tomato);
	background_down.setColor(ColorSet::OrangeRed);
}

MenuBar::MenuBar(MainFrame& main_frame, const wstring& title) :
	Wnd(nullwnd, Style()),
	_main_frame(main_frame),
	_title(*this),
	_close_button(*this),
	_system_wnd_drag_tracker(*this),
	_fading_animation(*this),
	_file_menu(*this, 5, L"File") {
	SetTitle(title);
	_main_frame->AddChildFixed(GetWnd());
}

bool MenuBar::Handler(Msg msg, Para para) {
	bool ret = true;

	if (msg == Msg::ChildHit) {
		const auto& msg_child_hit = GetMouseMsgChildHit(para);

		// If mouse hovers on child window, set the cursor.
		if (msg_child_hit.original_msg == Msg::MouseMove) {
			msg = msg_child_hit.original_msg;  // 
		}

		// Do not send any message to the title textbox.
		if (msg_child_hit.child == _title.GetWnd()) {
			msg = msg_child_hit.original_msg;
			ret = false;
		}
	}

	if (msg == Msg::MouseMove) {
		SetCursor(Cursor::Default);
	}

	_fading_animation.TrackMsg(msg, para);

	_system_wnd_drag_tracker.TrackMsg(msg, para);

	return ret;
}

void MenuBar::CloseButton::OnClick() {
	_menu_bar._main_frame.Destroy();
}

void MenuBar::CloseButton::AppendFigure(FigureQueue& figure_queue) {
	figure_queue.Push(Line::Create(Point(0, 0), Point(10, 10), ColorSet::White, 1), Rect(15, 10, 10, 10));
	figure_queue.Push(Line::Create(Point(10, 0), Point(0, 10), ColorSet::White, 1), Rect(15, 10, 10, 10));
}

MenuBar::Menu::Menu(MenuBar& menu_bar, uint start_position, const wstring& text) :
	ButtonWithText(nullwnd, Style(start_position)) {
	(*this)->SetText(text);
	menu_bar->AddChild(GetWnd());
}
