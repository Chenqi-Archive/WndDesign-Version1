//#define ComplexTest

#ifdef ComplexTest

#include "../WndDesign/WndDesign.h"

#pragma comment(lib, "../build/x64/Debug/WndDesign.lib")

using namespace WndDesign;

/////////////////////////////////////////////////////////////
////                      Resources                      ////
/////////////////////////////////////////////////////////////

// Image* background_line;



/////////////////////////////////////////////////////////////
////                    Window Styles                    ////
/////////////////////////////////////////////////////////////

WndStyle main_frame_style;
___ WndStyle menu_bar_style;
___ ___ ButtonStyle file_menu_style;
___ ___ ButtonStyle close_button_style;
___ ___ TextBoxStyle title_style;
___ ScrollWndStyle main_space_style;
___ ___ EditBoxStyle edit_box_style;
___ ___ ButtonStyle floating_button_style;

TableStyle file_menu_table_style;
___ ButtonStyle file_menu_table_button_style;  // Shared
___ WndStyle save_file_style;
___ ___ WndStyle saving_state_style;

MessageBoxStyle dialog_style;  // Shared

inline void StyleInit() {
	main_frame_style.size.normal(1000px, 550px).min(200px, 200px);
	main_frame_style.position.SetHorizontalCenter().SetVerticalCenter();
	{
		menu_bar_style.size.normal(100pct, 20px);
		menu_bar_style.position.SetHorizontalCenter();
		menu_bar_style.position.top(0px);
		menu_bar_style.background = ColorSet::Bisque;
		{
			file_menu_style.size.normal(60px, 26px);
			file_menu_style.position.left(2px);
			file_menu_style.position.SetVerticalCenter();
			file_menu_style.border.color(ColorSet::DarkSalmon);
			file_menu_style.border.width(3);
			file_menu_style.border.radius(13);
			file_menu_style.background = ColorSet::LemonChiffon;
			file_menu_style.background_hover = ColorSet::Moccasin;
			file_menu_style.background_down = ColorSet::BurlyWood;
			file_menu_style.align = TextBoxStyle::TextAlign::Center;
			file_menu_style.padding.SetAll(2px);

			close_button_style.size.normal(26px, 26px );
			close_button_style.position.right(2px);
			close_button_style.position.SetVerticalCenter();
			close_button_style.border.width(0px);
			close_button_style.background = ColorSet::LightPink;
			close_button_style.background_hover = ColorSet::Pink;
			close_button_style.background_down = ColorSet::Plum;
			close_button_style.align = TextBoxStyle::TextAlign::Center;
			close_button_style.font.size = 22;
			close_button_style.padding.SetAll(2px);

			title_style.size.normal(length_auto, 26px);
			title_style.position.left(64px).right(30px);
			title_style.position.SetVerticalCenter();
			title_style.background = ColorSet::Bisque;  // The same as menu bar.
			title_style.align = TextBoxStyle::TextAlign::Center;
			title_style.font.size = 22;
			title_style.padding.SetAll(2px);
		}
		main_space_style.size.normal(length_auto, length_auto);
		main_space_style.position.top(30px).left(0px).right(0px).down(0px);
		main_space_style.background = ColorSet::WhiteSmoke;
		main_space_style.border.width(0px);
		main_space_style.scroll_bar = nullptr;
		{
			edit_box_style.size.normal(300px, 200px).min(200px, 200px).max(800px, 1000px);
			edit_box_style.background = color_transparent;
			edit_box_style.border.width(5px);
			edit_box_style.border.color(ColorSet::LightGray);

			floating_button_style.size.SetFixed(50px, 50px);
			floating_button_style.position.left(-2pct).down(2pct);
			floating_button_style.is_fixed = true;
			floating_button_style.border.width(5).color(ColorSet::AliceBlue).radius(25);
			floating_button_style.background = ColorSet::Azure;
			floating_button_style.background_hover = ColorSet::Lavender;
			floating_button_style.background_down = ColorSet::Lavender;
		}
	}

	file_menu_table_style.size.normal(200px, 200px);
	file_menu_table_style.border.width(3px).color(ColorSet::Black);
	file_menu_table_style.background = ColorSet::Bisque;
	file_menu_table_style.grid_line.width(1).color(ColorSet::Gray);
	{
		file_menu_table_button_style.background = ColorSet::LemonChiffon;
		file_menu_table_button_style.background_hover = ColorSet::Moccasin;
		file_menu_table_button_style.background_down = ColorSet::BurlyWood;
		file_menu_table_button_style.align = ButtonStyle::TextAlign::Left;
		file_menu_table_button_style.padding.left(5px);
		
		{
			saving_state_style.size.normal(30px, 30px);
			saving_state_style.position.right(5px);
			saving_state_style.position.SetVerticalCenter();
		}
	}

	dialog_style.size.SetFixed(300px, 200px );
	dialog_style.position.SetHorizontalCenter().SetVerticalCenter();
	dialog_style.background = ColorSet::White;
	{
		TextBoxStyle& dialog_text_style = dialog_style.text_box_style;
		dialog_text_style.size.normal(100pct, 150px);
		dialog_text_style.position.top(0px).SetHorizontalCenter();
		dialog_text_style.padding.SetAll(10px);

		ButtonStyle& dialog_button_style = dialog_style.button_style;
		dialog_button_style.size.normal(50px, 30px);
		dialog_button_style.position.down(10px);
		dialog_button_style.border.width(3);
		dialog_button_style.background = ColorSet::LemonChiffon;
		dialog_button_style.background_hover = ColorSet::Moccasin;
		dialog_button_style.background_down = ColorSet::BurlyWood;
		// The horizontal position is automatically set by the MessageBox.
	}
}

/////////////////////////////////////////////////////////////
////               Window Message Handlers               ////
/////////////////////////////////////////////////////////////

Wnd* main_frame;
___ ScrollWnd* main_space;
___ ___ Button* floating_button;
Table* file_menu_table;
___ Wnd* save_file;
___ ___ WndBase* saving_state;
MessageBox* about_dialog;
MessageBox* confirm_dialog;

bool main_space_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftDown) {
		MouseMsg mouse_msg = GetMouseMsg(para);

		EditBox* edit_box = EditBox::Create(nullptr, &edit_box_style, nullptr);
		
		main_space->AddChild(edit_box, mouse_msg.point);
	}
}

bool floating_button_handler(WndBase* wnd, Msg msg, Para para) {
	// For moving the floating button.
	static bool has_mouse_down = false;
	static Point mouse_down_position;

	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		mouse_down_position = mouse_msg.point;
		has_mouse_down = true;
		return true;
	}
	if (msg == Msg::LeftUp) {
		has_mouse_down = false;
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!has_mouse_down) {
			return true;
		}
		Point current_position = mouse_msg.point;
		Vector moving_vector = current_position - mouse_down_position;
		Point origin_point = floating_button->ConvertToParentPoint(point_zero);
		Point new_point = origin_point + moving_vector;
		floating_button->SetPosition(new_point.x, new_point.y);
	}
	return true;
}

bool menu_bar_handler(WndBase* wnd, Msg msg, Para para) {
	// For moving the whole window.
	static bool has_mouse_down = false;
	static Point mouse_down_position;
	
	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		mouse_down_position = mouse_msg.point;
		has_mouse_down = true;
		return true;
	}
	if (msg == Msg::LeftUp) {
		has_mouse_down = false;
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!has_mouse_down) {
			return true;
		}
		Point current_position = mouse_msg.point;
		Vector moving_vector = current_position - mouse_down_position;
		//desktop.MoveWindow(main_frame, moving_vector);  // Not supported yet.
		//main_frame.Move(moving_vector);  // Not supported yet.
		Point main_frame_point = main_frame->ConvertToParentPoint(point_zero);
		Point main_frame_new_point = main_frame_point + moving_vector;
		main_frame->SetPosition(main_frame_new_point.x, main_frame_new_point.y);
	}
	return true;
}

bool file_menu_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		Point left_top = main_frame->ConvertToParentPoint(point_zero) + Vector{ 0, 30 };
		desktop->AddChild(file_menu_table);
		return true;
	}
	if (msg == Msg::LoseFocus) {
		desktop->RemoveChild(file_menu_table);
		return true;
	}
	return true;
}

bool close_button_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		desktop->AddModalDialogBox(confirm_dialog, main_frame);
	}
	return true;
}

bool open_file_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		// open file dialog.
	}
	return true;
}

bool save_file_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		save_file->AddChild(saving_state);
		
	}
	return true;
}

bool saving_state_handler(WndBase* wnd, Msg msg, Para para) {
	// Display a gif animation. When finished, remove itself from its parent.

	return true;
}

bool about_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		desktop->AddModalDialogBox(about_dialog, main_frame);  // All messages to main_frame will be blocked.
	}
	return true;
}

bool about_dialog_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		if (about_dialog->GetButtonID(para) == 0) {
			desktop->RemoveChild(about_dialog);
		}
	}
	return true;
}

bool confirm_dialog_handler(WndBase* wnd, Msg msg, Para para) {
	if (msg == Msg::LeftUp) {
		switch (confirm_dialog->GetButtonID(para)) {
			case 0:	desktop->Destroy(); break; // OK button. Destroy the program.
			case 1: desktop->RemoveChild(confirm_dialog); break;  // Cancle button. Remove but not destroy the dialog.
			default: break;
		}
		return true;
	}
	if (msg == Msg::LoseFocus) {
		desktop->RemoveChild(confirm_dialog);
	}
	return true;
}


////////////////////////////////////////////////////////////
////                 Window Object Tree                 ////
////////////////////////////////////////////////////////////

void Init() {
	StyleInit();

	main_frame = Wnd::Create(desktop, main_frame_style, nullptr); 
	{
		Wnd* menu_bar = Wnd::Create(main_frame, menu_bar_style, menu_bar_handler); 
		{
			Button* file_menu = Button::Create(menu_bar, file_menu_style, file_menu_handler, L"File");
			Button* close_button = Button::Create(menu_bar, close_button_style, close_button_handler, L"¡Á");
			TextBox* title = TextBox::Create(menu_bar, title_style, nullptr);
		}
		main_space = ScrollWnd::Create(main_frame, &main_space_style, main_space_handler);
		{
			floating_button = Button::Create(main_space, floating_button_style, floating_button_handler);
		}
	}

	file_menu_table = Table::Create(nullptr, file_menu_table_style);
	{
		Button* open_file = Button::Create(nullptr, file_menu_table_button_style, open_file_handler, L"Open");
		save_file = Wnd::Create(nullptr, save_file_style, nullptr);
		{
			Button* save = Button::Create(save_file, file_menu_table_button_style, save_file_handler, L"Save");
			saving_state = Wnd::Create(save_file, save_file_style, nullptr);
		}
		Button* settings = Button::Create(nullptr, file_menu_table_button_style, about_handler, L"Settings");
		Button* help = Button::Create(nullptr, file_menu_table_button_style, about_handler, L"°ïÖú");
		Button* about = Button::Create(nullptr, file_menu_table_button_style, about_handler, L"About");

		file_menu_table->AddChild(open_file);
		file_menu_table->AddChild(save_file);
		file_menu_table->AddChild(settings);
		file_menu_table->AddChild(help);
		file_menu_table->AddChild(about);
	}

	about_dialog = MessageBox::Create(dialog_style, about_dialog_handler); 
	about_dialog->SetText(L"WndDesign Program Demo\nThanks for using!");
	about_dialog->AddButton(L"OK");

	confirm_dialog = MessageBox::Create(dialog_style, confirm_dialog_handler);
	confirm_dialog->SetText(L"Are you going to close the program?");
	confirm_dialog->AddButton(L"Yes");
	confirm_dialog->AddButton(L"No");
}

void Final() {
	SafeDestroy(&main_frame); 
	SafeDestroy(&file_menu_table);
	SafeDestroy(&about_dialog);
	SafeDestroy(&confirm_dialog);
}

#endif