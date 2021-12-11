#include "clipboard_scroll_bar.h"

struct ScrollBarFrameStyle : public WndStyle {
	ScrollBarFrameStyle() {
		size.normal(100pct, 20px);
		position.down(0px).setHorizontalCenter();
		background.setColor(ColorSet::Lavender);
		render.z_index(0xff);  // The most top.
		//render.mouse_penetrate(true).opacity(0);
	}
};

struct ScrollBarSliderStyle : public ButtonStyle {
	ScrollBarSliderStyle() {
		size.normal(0px, 100pct);
		position.left(0px).top(0px);
		background.setColor(ColorSet::Thistle);
		background_hover.setColor(ColorSet::Plum);
		background_down.setColor(ColorSet::Orchid);
	}
};


SimpleScrollBarHorizontal::SimpleScrollBarHorizontal(ScrollWnd& parent) :
	Wnd(nullwnd, ScrollBarFrameStyle()), _parent(parent), _slider(*this) {
	_parent->AddChildFixed(GetWnd());  // For scrollWnd
}


SimpleScrollBarHorizontal::Slider::Slider(SimpleScrollBarHorizontal& frame) :
	Button(frame, ScrollBarSliderStyle()), _frame(frame), _mouse_drag_tracker(*this) {
}

bool SimpleScrollBarHorizontal::Slider::Handler(Msg msg, Para para) {
	Vector moving_vector = _mouse_drag_tracker.TrackMsg(msg, para);
	if (moving_vector != vector_zero) {
		Point point_on_frame = (*this)->ConvertToParentPoint(GetMouseMsg(para).point);
		Point origin_point = (*this)->ConvertToLayerPoint(point_zero);
		Point new_point = origin_point + moving_vector;

		uint frame_length = _frame->GetSize().width; if (frame_length == 0) { return true; }
		uint entire_length = _frame._parent->GetEntireRegion().size.width;
		int current_region_offset = moving_vector.x * static_cast<int>(entire_length) / static_cast<int>(frame_length);

		// The new position info is sent to the parent scroll window, 
		//   and the button's position will be recalculated when ViewUpdated is called.
		_frame._parent->ScrollView(Vector(current_region_offset, 0));

		_mouse_drag_tracker.SetDownPosition(point_on_frame - ((*this)->ConvertToParentPoint(point_zero) - point_zero));

		return true;
	}
	return false;
}

void SimpleScrollBarHorizontal::ViewUpdated() {
	if (_parent.GetWnd() == nullptr) { return; }

	// Get the new entire region and the current region.
	Rect entire_region = _parent->GetEntireRegion();
	Rect current_region = Rect(_parent->GetScrollPosition(), _parent->GetSize());

	// Calculate the new position and size of the slider button.
	uint entire_length = entire_region.size.width; if (entire_length == 0) { return; }
	uint current_length = current_region.size.width;
	uint current_offset = static_cast<uint>(current_region.point.x - entire_region.point.x);
	uint frame_length = (*this)->GetSize().width;

	uint slider_length = frame_length * current_length / entire_length;
	uint slider_offset = frame_length * current_offset / entire_length;

	if (current_length >= entire_length) {		// Not scrollable.
		auto& my_style = (*this)->GetStyle();
		if (my_style.render._opacity != 0) {
			my_style.render.opacity(0);
			(*this)->RenderStyleUpdated();
		}
	} else {
		auto& my_style = (*this)->GetStyle();
		if (my_style.render._opacity !=0xFF) {
			my_style.render.opacity(0xFF);
			(*this)->RenderStyleUpdated();
		}

		auto& style = _slider->GetStyle();
		style.size._normal.width = px(slider_length);
		style.position._left = px(slider_offset);
		_slider->RegionStyleUpdated();
	}
}