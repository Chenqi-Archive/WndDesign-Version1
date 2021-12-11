#include "SimpleScrollBar.h"


BEGIN_NAMESPACE(WndDesignHelper)


bool SimpleScrollBar::Slider::Handler(Ref<IWndBase*> wnd, Msg msg, Para para) {
	if (wnd != GetWnd()) { return false; }
	if (!IsMouseMsg(msg)) { return false; }

	MouseMsg mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		_mouse_down_position = mouse_msg.point;
		_has_mouse_down = true;
		(*this)->SetCapture();
		return true;
	}
	if (msg == Msg::LeftUp) {
		_has_mouse_down = false;
		(*this)->ReleaseCapture();
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (!_has_mouse_down) { return true; }
		Point current_position = mouse_msg.point;
		Point point_on_frame = (*this)->ConvertToParentPoint(current_position);
		Vector moving_vector = current_position - _mouse_down_position;

		// The new position info is sent to the parent scroll window, 
		//   and the button's position will be recalculated when ViewUpdated is called.
		uint frame_length = _frame->GetSize().height; if (frame_length == 0) { return true; }
		uint entire_length = _frame._entire_region.size.height;
		int visible_region_offset = moving_vector.y * static_cast<int>(entire_length) / static_cast<int>(frame_length);
		if (visible_region_offset == 0) { return true; }
		_frame._parent->MoveView({ 0, visible_region_offset });

		// The position may not be the same as before, should plus the offset.
		_mouse_down_position = point_on_frame - ((*this)->ConvertToParentPoint(point_zero) - point_zero);
		return true;
	}
	return false;
}

void SimpleScrollBar::Init() {
	Destroy();

	WndStyle frame_style;
	frame_style.size.normal(20px, 100pct);
	frame_style.position.right(0px).SetVerticalCenter();
	frame_style.background = ColorSet::Azure;
	frame_style.render.z_index(0xff);  // The most top.
	Wnd::Init(nullptr, frame_style);
	_parent->AddChildFixed(Wnd::GetWnd());

	ButtonStyle slider_style;
	slider_style.size.normal(100pct, 0px);
	slider_style.position.left(0px).top(0px);
	slider_style.background = ColorSet::Crimson;
	slider_style.background_hover = ColorSet::Crimson;
	slider_style.background_down = ColorSet::Crimson;
	_slider.Init(this, slider_style);
	ViewUpdated();
}

void SimpleScrollBar::Destroy() {
	_slider.Destroy();
	Wnd::Destroy();
}

void SimpleScrollBar::RefreshRegion() {
	if (_parent.IsEmpty()) {
		_entire_region = region_empty;
		_visible_region = region_empty;
	} else {
		_entire_region = _parent->GetEntireRegion();
		_visible_region.point = _parent->GetPosition();
		_visible_region.size = _parent->GetSize();
	}
}

void SimpleScrollBar::ViewUpdated() {
	// Refresh the entire region and the visible region.
	RefreshRegion();

	// Calculate the new position and size of the slider button.
	uint entire_length = _entire_region.size.height; if (entire_length == 0) { return; }
	uint visible_length = _visible_region.size.height;
	uint visible_offset = static_cast<uint>(_visible_region.point.y - _entire_region.point.y);

	uint slider_length_pct = 100 * visible_length / entire_length;
	uint slider_offset_pct = 100 * visible_offset / entire_length;

	_slider->SetRegion(100pct, pct(slider_length_pct), 0px, pct(slider_offset_pct));
}

bool SimpleScrollBar::Handler(Ref<IWndBase*> wnd, Msg msg, Para para) {
	if (wnd == GetWnd()) { return true; }
	return false;
}

END_NAMESPACE(WndDesignHelper)
