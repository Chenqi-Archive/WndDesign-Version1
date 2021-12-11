
class Wnd{
	RenderTarget _target;  // _target.GetSize();
	
}




Wnd* main_frame;


void Init(){
	RegionStyle region;
	CompositionStyle composition;
	
	main_frame = new Wnd(&color_white, main_frame_handler);
	
	region.size.normal = {1000, 600};
	region.size.min = {200, 200};
	region.size.max = {length_infinite, length_infinite}; // size_infinite
	region.position = {position_cener, position_center};
	
	composition.opacity = 0x80;
	composition.hint.background_transparent = true;
	composition.mouse_penetrate_content = false;
	composition.mouse_penetrate_background = true;
	
	desktop.AddChild(main_frame, region, composition);
	
	{
		menu_bar = new Wnd();
		
		main_frame.AddChild(menu_bar, region, composition);
		
		main_space = new ScrollWnd();
		
		textbox = new TextBox();
		
		main_space.AddChild(textbox, Rect)
	}
}


class MainSpace{
	
	
	void CreateTextBox(Point position);
}



