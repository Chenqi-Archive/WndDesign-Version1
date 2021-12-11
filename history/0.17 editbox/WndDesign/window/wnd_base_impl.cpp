#include "wnd_base_impl.h"
#include "../object/ObjectBase.h"
#include "../style/style_helper.h"

#include <cmath>


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWndBase, IWndBase::Deleter> 
IWndBase::Create(Ref<IWndBase*> parent, const WndStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<IWndBase, IWndBase::Deleter> wnd(new _WndBase_Impl(new WndStyle(style), object));
	wnd->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(wnd.get()); }
	return wnd;
}



inline int square(int x) { return x * x; }

inline bool PointInRoundedRectangle(const Rect& rect, uint radius, Point point) {
	if (!rect.Contains(point)) { return false; }
	uint max_radius = min(rect.size.width, rect.size.height) / 2;
	if (radius > max_radius) { radius = max_radius; }
	if (radius == 0) { return true; }
	int x1 = rect.point.x + static_cast<int>(radius);
	int	x2 = rect.point.x + static_cast<int>(rect.size.width - radius);
	int	y1 = rect.point.y + static_cast<int>(radius);
	int	y2 = rect.point.y + static_cast<int>(rect.size.height - radius);
	int x = point.x;
	int y = point.y;
	if (x < x1 && y < y1) {
		return static_cast<uint>(sqrt(square(x - x1) + square(y - y1))) <= radius ? true : false;
	}
	if (x < x1 && y > y2) {
		return static_cast<uint>(sqrt(square(x - x1) + square(y - y2))) <= radius ? true : false;
	}
	if (x > x2 && y < y1) {
		return static_cast<uint>(sqrt(square(x - x2) + square(y - y1))) <= radius ? true : false;
	}
	if (x > x2 && y > y2) {
		return static_cast<uint>(sqrt(square(x - x2) + square(y - y2))) <= radius ? true : false;
	}
	return true;
}

// Returns if the point at the background should be mouse transparent.
inline bool BackgroundHitTest(const WndStyle& style, const Rect& region, Point point) {
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
	this->DispatchMessage(Msg::Destroy, nullptr);
	delete _style;
}


bool _WndBase_Impl::SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) {
	bool has_reset = false;
	if (!width.IsAuto()) { _style->size._normal.width = width; has_reset = true; }
	if (!height.IsAuto()) { _style->size._normal.height = height; has_reset = true; }
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

const Rect _WndBase_Impl::CalculateRegionOnParent(Size parent_size) const {
	return CalculateActualRegion(GetStyle(), parent_size);
}


void _WndBase_Impl::SetCanvas(const Canvas& canvas) {
	bool layer_changed = GetLayer() != canvas.GetLayer();
	bool size_changed = GetSize() != canvas.GetSize();
	_canvas = canvas;
	if (layer_changed) {
		ResetLayerResourceManager(GetResourceManager()); // Virtual function call to reset layer's resource manager.
	}
	if (size_changed) {
		SizeChanged();  // Virtual function call to reset child window's size.
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

bool _WndBase_Impl::MouseHitTest(Point point) {
	// Check if the point falls out of the border or the background is transparent.
	if (PointInRoundedRectangle(Rect(point_zero, _canvas.GetSize()), _style->border._radius, point)
		&& BackgroundHitTest(GetStyle(), Rect(point_zero, _canvas.GetSize()), point)) {
		return true;
	}
	
	// Else call the user's handler to do hit test.
	// Msg::HitTest will be sent before a real mouse message is sent,
	//   if the handler returns false, the real message will not be sent.
	return DispatchMessage(Msg::HitTest, &point);
}

void _WndBase_Impl::SetCapture(_WndBase_Impl* wnd) {
	if (_capture_wnd != nullptr) {
		if (_capture_wnd == wnd) { return; }
		_capture_wnd->LoseCapture();
	}
	_capture_wnd = wnd;
	if (_parent != nullptr) { _parent->SetCapture(this); }
}

void _WndBase_Impl::ReleaseCapture() {
	if (_capture_wnd != nullptr) {
		if (_capture_wnd != this) {
			_capture_wnd->LoseCapture();
		}
		_capture_wnd = nullptr;
	}
	if (_parent != nullptr) { _parent->ReleaseCapture(); }
}

void _WndBase_Impl::LoseCapture() {
	if (_capture_wnd != nullptr) {
		if (_capture_wnd != this) {
			_capture_wnd->LoseCapture();
		}
		_capture_wnd = nullptr;
	}
	DispatchMessage(Msg::LoseCapture, nullptr);
}

void _WndBase_Impl::SetFocus(Ref<_WndBase_Impl*> wnd) {
	if (_focus_wnd != nullptr) {
		if (_focus_wnd == wnd) { return; }
		_focus_wnd->LoseFocus();
	}
	_focus_wnd = wnd;
	if (_parent != nullptr) { _parent->SetFocus(this); }
}

void _WndBase_Impl::ReleaseFocus() {
	if (_focus_wnd != nullptr) {
		if (_focus_wnd != this) {
			_focus_wnd->LoseFocus();
		}
		_focus_wnd = nullptr;
	}
	if (_parent != nullptr) { _parent->ReleaseFocus(); }
}

void _WndBase_Impl::LoseFocus() {
	if (_focus_wnd != nullptr) {
		if (_focus_wnd != this) {
			_focus_wnd->LoseFocus();
		}
		_focus_wnd = nullptr;
	}
	DispatchMessage(Msg::LoseFocus, nullptr);
}


END_NAMESPACE(WndDesign)