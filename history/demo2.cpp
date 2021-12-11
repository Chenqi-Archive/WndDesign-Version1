main_frame<Wnd> {
	menu_bar<Wnd> {
		file_menu<Button>"file" -> file_menu_table;
		title<TextBox>"CogArch";
		close_btn<Button> -> confirm_dialog;
	};
	main_space<ScrollWnd>{
		...<EditBox>;
	};
}

file_menu_table<Table>{
	open_file<Button>"Open" -> open_file_dialog;
	save_file<Wnd>{
		save<Button>"Save";
		saving_state<BaseWnd>;
	}
	settings<Button>"Settings" -> about_dialog;
	help<Button>"Help" -> about_dialog;
	about<Button>"About" -> about_dialog;	
}

about_dialog<Wnd>{
	text<TextBox>"CogArch";
	ok<Button>"OK";
}

confirm_dialog<Wnd>{
	text<TextBox>"CogArch";
	ok<Button>"Ok";
	cancle<Button>"Cancle";
}



Wnd* main_frame;
___	Wnd* menu_bar;
___ ___	Button* file_menu;
___ ___	TextBox* title;
___ ___	Button* close_btn;
___ ScrollWnd* main_space;

Table* file_menu_table;
___ Button* open_file;
___ Wnd* save_file;
___ ___ TextBox* save_text;



template<int a, class WndType, class StyleType>
class WndFrame{
	WndType* wnd;
	StyleType style;
	
	static bool WndMap()
	
	virutal bool Handler(Msg msg, Para para);
}

class MainWnd : WndFrame{
	
}