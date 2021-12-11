#pragma once

#include "WndDesign.h"

#include "../block/block.h"

#include <list>

using std::list;


class ClipboardView;

class ClipboardItem : public TextBox {
	ClipboardView& _clipboard_view;
	bool _has_mouse_down = false;
	Point _mouse_down_position = point_zero;
public:
	Block& _block;
	list<ClipboardItem>::iterator iterator;
	ClipboardItem(ClipboardView& clipboard_view, int position, Block& block);
	bool Handler(Msg msg, Para para);
};
