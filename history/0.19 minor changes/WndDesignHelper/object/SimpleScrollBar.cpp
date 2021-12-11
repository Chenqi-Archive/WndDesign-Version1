#include "SimpleScrollBar.h"


BEGIN_NAMESPACE(WndDesignHelper)


struct ScrollBarFrameStyle : public WndStyle {
	ScrollBarFrameStyle() {
		size.normal(20px, 100pct);
		position.right(0px).setVerticalCenter();
		background.setColor(ColorSet::Azure);
		render.z_index(0xff);  // The most top.
	}
};

struct ScrollBarSliderStyle : public ButtonStyle {
	ScrollBarSliderStyle() {
		size.normal(100pct, 0px);
		position.left(0px).top(0px);
		background.setColor(ColorSet::Crimson);
		background_hover.setColor(ColorSet::Crimson);
		background_down.setColor(ColorSet::Crimson);
	}
};


SimpleScrollBar::Slider::Slider(SimpleScrollBar& frame) : 
	Button(frame, ScrollBarSliderStyle()), _frame(frame), _mouse_drag_tracker(*this) {}

bool SimpleScrollBar::Slider::Handler(Msg msg, Para para) {
	Vector moving_vector = _mouse_drag_tracker.TrackMsg(msg, para);
	if (moving_vector != vector_zero) {
		Point point_on_frame = (*this)->ConvertToParentPoint(GetMouseMsg(para).point);
		Point origin_point = (*this)->ConvertToLayerPoint(point_zero);
		Point new_point = origin_point + moving_vector;

		uint frame_length = _frame->GetSize().height; if (frame_length == 0) { return true; }
		uint entire_length = _frame._parent->GetEntireRegion().size.height;
		int current_region_offset = moving_vector.y * static_cast<int>(entire_length) / static_cast<int>(frame_length);

		// The new position info is sent to the parent scroll window, 
		//   and the button's position will be recalculated when ViewUpdated is called.
		_frame._parent->ScrollView(Vector(0, current_region_offset));

		_mouse_drag_tracker.SetDownPosition(point_on_frame - ((*this)->ConvertToParentPoint(point_zero) - point_zero));

		return true;
	}
	return false;
}

SimpleScrollBar::SimpleScrollBar(ScrollWnd& parent) : 
	Wnd(nullwnd, ScrollBarFrameStyle()), _parent(parent), _slider(*this) {
	_parent->AddChildFixed(GetWnd());
}

void SimpleScrollBar::ViewUpdated() {
	if (_parent.GetWnd() == nullptr) { return; }

	// Get the new entire region and the current region.
	Rect entire_region = _parent->GetEntireRegion();
	Rect current_region = Rect(_parent->GetScrollPosition(), _parent->GetSize());

	// Calculate the new position and size of the slider button.
	uint entire_length = entire_region.size.height; if (entire_length == 0) { return; }
	uint current_length = current_region.size.height;
	uint current_offset = static_cast<uint>(current_region.point.y - entire_region.point.y);
	uint frame_length = (*this)->GetSize().height;

	uint slider_length = frame_length * current_length / entire_length;
	uint slider_offset = frame_length * current_offset / entire_length;

	auto& style = _slider->GetStyle();
	style.size._normal.height = px(slider_length);
	style.position._top = px(slider_offset);
	_slider->RegionStyleUpdated();
}


END_NAMESPACE(WndDesignHelper)