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
		int current_region_offset = moving_vector.y * static_cast<int>(entire_length) / static_cast<int>(frame_length);
		if (current_region_offset == 0) { return true; }
		_frame._parent->MoveView({ 0, current_region_offset });

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
}

void SimpleScrollBar::Destroy() {
	_slider.Destroy();
	Wnd::Destroy();
}

void SimpleScrollBar::ViewUpdated() {
	// Get the new entire region and the current region.
	Rect entire_region = _parent->GetEntireRegion();
	Rect current_region = _parent->GetCurrentRegion();

	// Calculate the new position and size of the slider button.
	uint entire_length = entire_region.size.height; if (entire_length == 0) { return; }
	uint current_length = current_region.size.height;
	uint current_offset = static_cast<uint>(current_region.point.y - entire_region.point.y);
	uint frame_length = (*this)->GetSize().height;

	uint slider_length = frame_length * current_length / entire_length;
	uint slider_offset = frame_length * current_offset / entire_length;

	_slider->SetRegion(100pct, px(slider_length), 0px, px(slider_offset));
}

bool SimpleScrollBar::Handler(Ref<IWndBase*> wnd, Msg msg, Para para) {
	if (wnd == GetWnd()) { return true; }
	return false;
}

END_NAMESPACE(WndDesignHelper)
