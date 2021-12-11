#pragma once

#include "clipboard_item.h"
#include "clipboard_scroll_bar.h"
#include "widget/scroll_animation.h"


class MainFrame;

class ClipboardView : public ScrollWnd {
private:
	MainFrame& _main_frame;
	list<ClipboardItem> _items;
	SimpleScrollBarHorizontal _scroll_bar;

	ScrollAnimation _scroll_animation;
	static const int _scroll_delta_for_mouse_wheel = 120; // px
	static const int _scroll_delta_for_key = 60;  // px


public:
	ClipboardView(MainFrame& main_frame);

	bool Handler(Msg msg, Para para);

	void AddItem(Block& block);
	void DragItem(ClipboardItem& item, Point position_on_item);
	void DropItem(ClipboardItem& item);
};