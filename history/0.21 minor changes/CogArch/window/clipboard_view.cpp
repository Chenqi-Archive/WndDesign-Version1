#include "clipboard_view.h"
#include "main_frame.h"

struct ClipboardViewStyle : public ScrollWndStyle {
	ClipboardViewStyle() {
		size.setFixed(100pct, 150px);
		position.left(0px).down(0px);
		border.width(2px).color(ColorSet::DarkGray);
		render.mouse_penetrate(true);
		background.setColor(ColorSet::LightGray);
		render.z_index(0x7f).opacity(0x00);
	}
};


ClipboardView::ClipboardView(MainFrame& main_frame) :
	ScrollWnd(main_frame, ClipboardViewStyle()),
	_main_frame(main_frame),
	_scroll_bar(*this) ,
	_scroll_animation(*this) { 
}

bool ClipboardView::Handler(Msg msg, Para para) {
	if (msg == Msg::HitTest) {
		if (GetMouseMsg(para).point.y >= 120 && !_items.empty()) {
			if ((*this)->GetStyle().render._opacity != 0xFF) { (*this)->SetOpacity(0xFF); }  // Show window.
			return false;  // The window is hit!
		}
		return true; // Not hit.
	}

	if (msg == Msg::MouseLeave) {
		if ((*this)->GetStyle().render._opacity != 0x00) { (*this)->SetOpacity(0x00); } // Hide window.
		return true;
	}

	ConvertChildHitMsgToOriginalMsg(msg, para);

	// For horizontal scrolling.
	if (msg == Msg::MouseWheel) {
		const MouseMsg& msg = GetMouseMsg(para);
		_scroll_animation.AddUp(-msg.wheel_delta * _scroll_delta_for_mouse_wheel / 120);
		// Wheel_delta is positive when scroll away, so the view_delta is negative.
		return true;
	}

	if (msg == Msg::KeyDown) { // For up and down keys.
		KeyMsg key_msg = GetKeyMsg(para);
		if (key_msg.key == Key::Down) {
			_scroll_animation.AddUp(_scroll_delta_for_key);
		} else if (key_msg.key == Key::Up) {
			_scroll_animation.AddUp(-_scroll_delta_for_key);
		}
		return true;
	}

	if (int wheel_delta = _scroll_animation.TrackTimerMsg(msg, para); wheel_delta != 0) {
		(*this)->ScrollView(Vector(wheel_delta, 0));
		return true;
	}

	if (msg == Msg::Scroll) {
		_scroll_bar.ViewUpdated();
		return true;
	}

	return true;
}

void ClipboardView::AddItem(Block& block) {
	auto& item = _items.emplace_back(*this, static_cast<int>(_items.size() * 185 + 5), block);
	item.iterator = --_items.end();

	// Extend the entire region.
	ScrollWndStyle::RegionStyle entire_region;
	entire_region._width = px(static_cast<uint>(_items.size() * 185 + 5));
	(*this)->SetEntrieRegion(entire_region);
}

void ClipboardView::DragItem(ClipboardItem& item, Point position_on_item) {
	_main_frame.DragItem(item);
}

void ClipboardView::DropItem(ClipboardItem& item) {
	int position = item->GetRegionOnLayer().point.x;
	auto it = item.iterator;
	_items.erase(it++);
	for (; it != _items.end(); ++it) {
		(*it)->SetPosition(position, 5);
		position += 185;
	}

	ScrollWndStyle::RegionStyle entire_region;
	entire_region._width = px(static_cast<uint>(position));
	(*this)->SetEntrieRegion(entire_region);
}