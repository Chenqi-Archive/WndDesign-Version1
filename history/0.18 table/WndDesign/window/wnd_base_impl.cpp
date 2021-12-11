#include "wnd_base_impl.h"
#include "../object/ObjectBase.h"
#include "../style/style_helper.h"
#include "../geometry/geometry_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWndBase, IWndBase::Deleter> 
IWndBase::Create(Ref<IWndBase*> parent, const WndStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<IWndBase, IWndBase::Deleter> wnd(new _WndBase_Impl(new WndStyle(style), object));
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
	DetachFromParent();
	delete _style;
}


bool _WndBase_Impl::SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) {
	bool has_reset = false;
	if (!width.IsAuto()) { _style->size._normal.width = width;  _style->auto_resize._width_auto_resize = false; has_reset = true; }
	if (!height.IsAuto()) { _style->size._normal.height = height; _style->auto_resize._height_auto_resize = false; has_reset = true; }
	if (!left.IsAuto()) { _style->position._left = left; has_reset = true; }
	if (!top.IsAuto()) { _style->position._top = top; has_reset = true;}
	if (!has_reset) { return false; }
	if (_parent != nullptr) { return _parent->UpdateChildRegion(this); }
	return false;
}

void _WndBase_Impl::SetOpacity(uchar opacity) {
	_style->render.opacity(opacity);
	if (_parent != nullptr) { 
		_parent->UpdateChildRenderStyle(this); 
	}
}

void _WndBase_Impl::SetBackground(const Background& background) {
	_style->background = background;
	_canvas.RegionUpdated(Rect{ point_zero, _canvas.GetSize() });
}

const Rect _WndBase_Impl::CalculateRegionOnParent(Size parent_size) {
	WndStyle& style = const_cast<WndStyle&>(GetStyle());

	_Interval horizontal(0, 0), vertical(0, 0);

	// First calculate the dimensions that won't auto resize.
	if (!style.auto_resize._width_auto_resize) { horizontal = CalculateActualWidth(style, parent_size.width); }
	if (!style.auto_resize._height_auto_resize) { vertical = CalculateActualHeight(style, parent_size.height); }

	// If does not auto resize, return the region directly.
	if (style.auto_resize.NotAutoResize()) { return _Make_Rect_From_Interval(horizontal, vertical); }


	Size min_size = CalculateMinSize(style, parent_size);
	Size max_size = CalculateMaxSize(style, parent_size);

	// If both auto resize, resize to content.
	if (style.auto_resize.BothAutoResize()) {
		Size size = AdjustSizeToContent(min_size, max_size);
		style.size._normal.width = px(size.width);
		style.size._normal.height = px(size.height);
	}
	
	// If only height auto resize, resize the height accroding to the width.
	if (style.auto_resize.HeightAutoResizeOnly()) {
		uint height = AdjustHeightToContent(min_size.height, max_size.height, horizontal.length);
		style.size._normal.height = px(height);
	}

	// If only width auto resize, resize the width according to the height.
	if (style.auto_resize.WidthAutoResizeOnly()) {
		uint width = AdjustWidthToContent(min_size.width, max_size.width, vertical.length);
		style.size._normal.width = px(width);
	} 

	// Last calculate the dimensions that has resized.
	if (style.auto_resize._width_auto_resize) { horizontal = CalculateActualWidth(style, parent_size.width); }
	if (style.auto_resize._height_auto_resize) { vertical = CalculateActualHeight(style, parent_size.height); }

	return _Make_Rect_From_Interval(horizontal, vertical);
}


void _WndBase_Impl::SetCanvas(const Canvas& canvas) {
	bool layer_changed = GetLayer() != canvas.GetLayer();
	bool size_changed = GetSize() != canvas.GetSize();
	_canvas = canvas;
	if (layer_changed) {
		ResetLayerResourceManager(GetResourceManager()); // Virtual function call to reset layer's resource manager.
	}
	if (size_changed) {
		BlockComposition();  // No composition is allowed for optimization, the window will be composited by its parent later.
		SizeChanged();  // Virtual function call to reset child window's size.
		DispatchMessage(Msg::Scroll, nullptr);	// For redrawing scrollbar.
		AllowComposition();
	}
}

void _WndBase_Impl::Composite(Rect region_to_update) const {
	FigureQueue figure_queue;
	figure_queue.Push(
		RoundedRectangle::Create(_style->border._radius, _style->border._width, _style->border._color, _style->background), 
		Rect(point_zero, _canvas.GetSize())
	);
	DrawFigureQueue(figure_queue, region_to_update);
}


void _WndBase_Impl::SetChildCapture(Ref<_WndBase_Impl*> child) {
	if (_capture_wnd != nullptr) {
		if (_capture_wnd == child) { return; }
		_capture_wnd->LoseCapture();
	}
	_capture_wnd = child;
	SetCapture();  // Set myself to be captured as well.
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
	SetFocus();
}

void _WndBase_Impl::ReleaseChildFocus() {
	if (_focus_wnd != nullptr) {
		_focus_wnd->LoseFocus();
		_focus_wnd = nullptr;
	}
	ReleaseFocus(); 
}

bool _WndBase_Impl::DispatchMessage(Msg msg, Para para) const {
	if (IsMouseMsg(msg)) {
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
			// Convert to point to child window.
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
		if (_focus_wnd != nullptr) {
			return _focus_wnd->DispatchMessage(msg, para);
		}
	}

	if (_object != nullptr) { 
		return _object->Handler(msg, para); 
	}

	return false;
}

bool _WndBase_Impl::MouseHitTest(Point point) const {
	// Check if the point falls out of the border or the background is transparent.
	if (PointInRoundedRectangle(Rect(point_zero, _canvas.GetSize()), _style->border._radius, point) && 
		HitTestBackground(GetStyle(), Rect(point_zero, _canvas.GetSize()), point)) {
		return true;
	}

	// Else call the user's handler to do hit test.
	// Msg::HitTest will be sent before a real mouse message is sent. If false is returned, the real message will not be sent.
	return DispatchMessage(Msg::HitTest, &point);
}


END_NAMESPACE(WndDesign)