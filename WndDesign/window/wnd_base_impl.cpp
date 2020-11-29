#include "wnd_base_impl.h"
#include "../object/ObjectBase.h"
#include "../style/style_helper.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWndBase> IWndBase::Create(Ref<IWndBase*> parent, const WndStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<IWndBase> wnd = std::make_unique<_WndBase_Impl>(new WndStyle(style), object);
	if (parent != nullptr) { parent->AddChild(wnd.get()); }
	return wnd;
}



// Returns if the point at the background should be mouse transparent.
inline bool HitTestBackground(const WndStyle& style, const Rect& region, Point point) {
	if (style.render._opacity != 0xFF && style.render._mouse_penetrate) {
		return false;
	}
	if (!style.background.IsOpaque(point) && style.render._mouse_penetrate_background) {
		return false;
	}
	return true;
}


_WndBase_Impl::_WndBase_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object) :
	_parent(nullptr),
	_style(style),
	_canvas(),
	_object(object),
	_last_child(nullptr),
	_capture_wnd(nullptr),
	_focus_wnd(nullptr){
}

_WndBase_Impl::~_WndBase_Impl() {
	// This trick allows your window destroy itself when handling the message.
	if (_delayed_update != nullptr) { _delayed_update->is_parent_valid = false; }
	DetachFromParent();
	delete _style;
}

const Rect _WndBase_Impl::CalculateRegionOnParent(Size parent_size) {
	WndStyle& style = const_cast<WndStyle&>(GetStyle());

	_Interval horizontal(0, 0), vertical(0, 0);

	// First calculate the dimensions that won't auto resize.
	if (!style.auto_resize._width_auto_resize) { horizontal = CalculateActualWidth(style, parent_size.width); }
	if (!style.auto_resize._height_auto_resize) { vertical = CalculateActualHeight(style, parent_size.height); }

	// If does not auto resize, return the region directly.
	if (style.auto_resize._NotAutoResize()) { return _Make_Rect_From_Interval(horizontal, vertical); }


	Size min_size = CalculateMinSize(style, parent_size);
	Size max_size = CalculateMaxSize(style, parent_size);

	// If both auto resize, resize to content.
	if (style.auto_resize._BothAutoResize()) {
		Size size = AdjustSizeToContent(min_size, max_size);
		style.size._normal.width = px(size.width);
		style.size._normal.height = px(size.height);
	}
	
	// If only height auto resize, resize the height accroding to the width.
	if (style.auto_resize._HeightAutoResizeOnly()) {
		uint height = AdjustHeightToContent(min_size.height, max_size.height, horizontal.length);
		style.size._normal.height = px(height);
	}

	// If only width auto resize, resize the width according to the height.
	if (style.auto_resize._WidthAutoResizeOnly()) {
		uint width = AdjustWidthToContent(min_size.width, max_size.width, vertical.length);
		style.size._normal.width = px(width);
	} 

	// Last calculate the dimensions that has resized.
	if (style.auto_resize._width_auto_resize) { horizontal = CalculateActualWidth(style, parent_size.width); }
	if (style.auto_resize._height_auto_resize) { vertical = CalculateActualHeight(style, parent_size.height); }

	return _Make_Rect_From_Interval(horizontal, vertical);
}


void _WndBase_Impl::SetCanvas(const Canvas& canvas, Rect visible_region) {
	bool layer_changed = GetLayer() != canvas.GetLayer();
	bool size_changed = GetSize() != canvas.GetSize();
	_canvas = canvas;
	if (size_changed) {
		BeginRegionUpdate();  // No composition is allowed for optimization, the window will be composited by its parent later.
		SizeChanged();  // Virtual function call to reset child windows and layers' size.
		DispatchMessage(Msg::Scroll, nullptr);	// For redrawing scrollbar.
		EndRegionUpdate();
	}
	if (layer_changed) {
		ResetLayerResourceManager(GetResourceManager()); // Virtual function call to reset layer's resource manager.
	}
	RefreshVisibleRegion(visible_region);
}

void _WndBase_Impl::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const {
	auto& style = GetStyle();
	figure_queue.append(
		new RoundedRectangle(style.border._radius, style.border._width, style.border._color, style.background),
		Rect(point_zero, GetSize())
	);
}


void _WndBase_Impl::SetChildCapture(Ref<_WndBase_Impl*> child) {
	if (_capture_wnd != nullptr) {
		if (_capture_wnd == child) { return; }
		_capture_wnd->LoseCapture();
	}
	_capture_wnd = child;
	if (_parent != nullptr) { _parent->SetChildCapture(this); }
}

void _WndBase_Impl::ReleaseChildCapture() {
	if (_capture_wnd != nullptr) {
		_capture_wnd->LoseCapture();
		_capture_wnd = nullptr;
	}
	ReleaseCapture();  // Release myself's capture.
}

void _WndBase_Impl::SetChildFocus(Ref<_WndBase_Impl*> child) {
	if (_focus_wnd != nullptr) {
		if (_focus_wnd == child) { return; }
		_focus_wnd->LoseFocus();
	}
	_focus_wnd = child;
	if (_parent != nullptr) { _parent->SetChildFocus(this); }
}

void _WndBase_Impl::ReleaseChildFocus() {
	if (_focus_wnd != nullptr) {
		_focus_wnd->LoseFocus();
		_focus_wnd = nullptr;
	}
	ReleaseFocus(); 
}

void _WndBase_Impl::DispatchMessage(Msg msg, Para para) {

	// Dangourous! The window may destroy itself when handling messages.
	// If the window destructs, EndRegionUpdate() should not be called.
	DelayedRegionUpdate delayed_update(this);


	if (_block_child_msg == true) { HandleMessage(msg, para); return; }

	if (IsMouseMsg(msg) && _capture_wnd != this) {
		MouseMsg mouse_msg = GetMouseMsg(para);

		// Get the child window to receive the message.
		// First check the capture window.
		Ref<_WndBase_Impl*> child;

		if (_capture_wnd != nullptr) {
			child = _capture_wnd;
		} else {
			child = HitTestChild(mouse_msg.point);
		}

		// Check the last tracked child window.
		if (child != _last_child) {
			if (_last_child != nullptr) {
				_last_child->DispatchMessage(Msg::MouseLeave, nullptr);
			}
			_last_child = child;
		}

		// Deliver the message to child window.
		if (child != nullptr) {
			// First before send the message to child, notify the window with the message.
			MouseMsgChildHit mouse_msg_child_hit;
			mouse_msg_child_hit.point = mouse_msg.point;
			mouse_msg_child_hit._key_state = mouse_msg._key_state;
			mouse_msg_child_hit.wheel_delta = mouse_msg.wheel_delta;
			mouse_msg_child_hit.original_msg = msg;
			mouse_msg_child_hit.child = child;
			if (HandleMessage(Msg::ChildHit, &mouse_msg_child_hit) == false) { return; }

			// Deliver the message child window.
			mouse_msg.point = mouse_msg.point - (child->ConvertToParentPoint(point_zero) - point_zero);
			return child->DispatchMessage(msg, &mouse_msg);
		}
	} 
	
	else if (msg == Msg::MouseLeave) {
		if (_last_child != nullptr) {
			_last_child->DispatchMessage(Msg::MouseLeave, nullptr);
			_last_child = nullptr;
		}
	} 
	
	else if (IsKeyboardMsg(msg)) {
		if (_focus_wnd != nullptr && _focus_wnd != this) {
			return _focus_wnd->DispatchMessage(msg, para);
		}
	}

	HandleMessage(msg, para);
}

bool _WndBase_Impl::MouseHitTest(Point point) const {
	// Check if the point falls out of the border or the background is transparent.
	if (PointInRoundedRectangle(Rect(point_zero, _canvas.GetSize()), _style->border._radius, point) && 
		HitTestBackground(GetStyle(), Rect(point_zero, _canvas.GetSize()), point)) {
		return true;
	}

	// Else call the user's handler to do hit test.
	// Msg::HitTest will be sent before a real mouse message is sent. If true is returned, the real message will not be sent.
	return !HandleMessage(Msg::HitTest, &point);
}


END_NAMESPACE(WndDesign)