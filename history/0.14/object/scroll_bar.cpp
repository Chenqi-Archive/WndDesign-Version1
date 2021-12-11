#include "scroll_bar.h"
#include "../window/scroll_wnd.h"
#include "../window/wnd.h"
#include "../window/button.h"

BEGIN_NAMESPACE(WndDesign)

ScrollBar::ScrollBar() :_parent(nullptr), _entire_region(region_empty), _visible_region(region_empty) {}

void ScrollBar::Clear() {
	_parent = nullptr;
	_entire_region = region_empty;
	_visible_region = region_empty;
}

void ScrollBar::RefreshRegion() {
	if (_parent == nullptr) {
		_entire_region = region_empty;
		_visible_region = region_empty;
	} else {
		_entire_region = _parent->GetEntireRegion();
		_visible_region.point = _parent->GetPosition();
		_visible_region.size = _parent->GetSize();
	}
}



SimpleScrollBar::SimpleScrollBar() :
	ScrollBar(), _frame(nullptr), _slider(nullptr),
	_has_mouse_down(false), _mouse_down_position(point_zero) {
}

void SimpleScrollBar::Init(Ref ScrollWnd* parent) {
	Clear();
	_parent = parent;

	WndStyle frame_style;
	frame_style.size.normal(10px, 100pct);
	frame_style.position.right(0px).SetVerticalCenter();
	frame_style.background = ColorSet::Azure;
	frame_style.render.z_index(0xff);  // The most top.
	_frame = Wnd::Create(nullptr, frame_style, nullptr);
	parent->AddChildFixed(_frame);

	ButtonStyle slider_style;
	slider_style.size.normal(100pct, 0px);
	slider_style.position.left(0px).top(0px);
	_slider = Button::Create(_frame, slider_style, nullptr);

	BindWindow(_frame);
	BindWindow(_slider);
}

void SimpleScrollBar::Clear() {
	SafeDestroy(_frame);
	SafeDestroy(_slider);
	_has_mouse_down = false;
	_mouse_down_position = point_zero;
	ScrollBar::Clear();
}

void SimpleScrollBar::ViewUpdated() {
	// Refresh the entire region and the visible region.
	ScrollBar::RefreshRegion();

	// Calculate the new position and size of the slider button.
	uint entire_length = _entire_region.size.height; if (entire_length == 0) { return; }
	uint visible_length = _visible_region.size.height;
	uint visible_offset = static_cast<uint>(_visible_region.point.y - _entire_region.point.y);

	uint slider_length_pct = 100 * visible_length / entire_length;
	uint slider_offset_pct = 100 * visible_offset / entire_length;

	_slider->SetRegion(0px, pct(slider_offset_pct), 100pct, pct(slider_length_pct));
}

bool SimpleScrollBar::Handler(Ref WndBase* wnd, Msg msg, Para para) {
	if (wnd == _frame) { return true; }
	if (wnd != _slider) { return false; }
	
	// Handler for slider.

	if (!IsMouseMsg(msg)) { return false; }
	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		_mouse_down_position = mouse_msg.point;
		_has_mouse_down = true;
		_slider->SetCapture();
		return true;
	}
	if (msg == Msg::LeftUp) {
		_has_mouse_down = false;
		_slider->ReleaseCapture();
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!_has_mouse_down) { return true; }
		Point current_position = mouse_msg.point;
		Vector moving_vector = current_position - _mouse_down_position;

		// The new position info is sent to the parent scroll window, 
		//   and the button's position will be recalculated when ViewUpdated is called.
		uint frame_length = _frame->GetSize().height; if (frame_length == 0) { return true; }
		uint entire_length = _entire_region.size.height;
		int visible_region_offset = moving_vector.y * static_cast<int>(entire_length) / static_cast<int>(frame_length);
		_parent->MoveView({ 0, visible_region_offset });
		return true;
	}
	return true;
}

END_NAMESPACE(WndDesign)