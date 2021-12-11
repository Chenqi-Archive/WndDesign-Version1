#include "clipboard_item.h"
#include "clipboard_view.h"


struct ClipboardItemStyle : public TextBoxStyle {
	ClipboardItemStyle(int pos) {
		size.setFixed(180px, 140px);
		position.left(px(pos)).top(5px);
		background.setColor(Color(ColorSet::LightBlue, 0x7F));
		padding.setAll(0px);
		paragraph.word_wrap(WordWrap::Character).line_height(120pct);
		font.family(L"µÈÏß").size(20px).color(ColorSet::Black);
	}
};

ClipboardItem::ClipboardItem(ClipboardView& clipboard_view, int position, Block& block) :
	TextBox(nullwnd, ClipboardItemStyle(position), block.Content()) , _clipboard_view(clipboard_view), _block(block){
	clipboard_view->AddChildStatic(GetWnd());
}

bool ClipboardItem::Handler(Msg msg, Para para) {
	if (msg == Msg::LeftDown) {
		_has_mouse_down = true; _mouse_down_position = GetMouseMsg(para).point;
	} else if (msg == Msg::LeftUp) {
		_has_mouse_down = false;
	} else if (msg == Msg::MouseMove && _has_mouse_down) {
		_clipboard_view.DragItem(*this, _mouse_down_position);
		_has_mouse_down = false;
	}
	return true;
}
