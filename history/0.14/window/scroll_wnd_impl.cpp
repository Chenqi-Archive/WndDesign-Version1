#include "scroll_wnd_impl.h"
#include "../object/scroll_bar.h"
#include "../style/style_helper.h"

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc ScrollWnd* ScrollWnd::Create(Ref WndBase* parent, const ScrollWndStyle& style, Handler handler) {
	_Scroll_Wnd_Impl* wnd = new _Scroll_Wnd_Impl(new WndStyle(style), handler, style.entire_region, style.scroll_bar);
	wnd->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(wnd); }
	return wnd;
}




_Scroll_Wnd_Impl::_Scroll_Wnd_Impl(Alloc WndStyle* style, Handler handler,
								   ScrollWndStyle::RegionStyle entire_region, Ref ScrollBar* scroll_bar) :
	_Wnd_Impl(style, handler, entire_region), _scroll_bar(scroll_bar){
}

void _Scroll_Wnd_Impl::SetCanvas(const Canvas& canvas) {
	bool size_changed = (GetSize() != canvas.GetSize());
	_WndBase_Impl::SetCanvas(canvas);
	// If size not changed, no need to recalculate canvas for child windows.
	if (!size_changed) { return; }

	// Recalculate the base layer's entire region.
	bool accessible_region_changed = _base_layer.SetCompositeSize(GetSize());
	if (accessible_region_changed) {
		_base_layer.ResetChildRegion(); // Recalculate all child window's region.
	}

	// Reset the top layers' region.
	for (auto& it : _top_layers) {
		if (it.IsSticky()) {
			// Recalculate the relative region of sticky layer.
			if (accessible_region_changed) {
				it.SetRelativeRegion(CalculateActualRegion(it.GetWnd()->GetStyle(), _base_layer.GetAccessibleRegion()));
			}
			Rect new_region = CalculateStickyLayerRegion(it.GetRelativeRegion(), it.GetPadding());
			if (new_region != it.GetCompositeRegion()) {
				it.SetCompositeRegion(new_region);
			}
		} else {
			Rect new_region = CalculateActualRegion(it.GetWnd()->GetStyle(), GetSize());
			it.SetCompositeRegion(new_region);
		}
	}
}

const Rect _Scroll_Wnd_Impl::CalculateStickyLayerRegion(Rect relative_region, Padding padding) {
	// Calculate the bounding region.
	Rect bounding_region = CalculateBoundingRegion(padding, GetSize());
	// Bound the region.
	relative_region.point = _base_layer.ConvertToParentPoint(relative_region.point);
	return BoundRectInRegion(relative_region, bounding_region);
}

void _Scroll_Wnd_Impl::SetPosition(Point point) {
	if (!_base_layer.SetCurrentPoint(point)) {
		return;  // The current point has not changed.
	}

	if (_scroll_bar != nullptr) {
		_scroll_bar->ViewUpdated();
	}

	// Calculate sticky windows' new region.
	for (auto& it : _top_layers) {
		if (it.IsSticky()) {
			Rect region_on_parent = CalculateStickyLayerRegion(it.GetRelativeRegion(), it.GetPadding());
			if (region_on_parent != it.GetCompositeRegion()) {
				it.SetCompositeRegion(region_on_parent);
			}
		}
	}

	RegionUpdated(region_infinite);	// Composite.
}

void _Scroll_Wnd_Impl::SetScrollBar(Ref ScrollBar* scroll_bar) {
	if (_scroll_bar != nullptr) {
		_scroll_bar->Clear();
	}
	_scroll_bar = scroll_bar;
	if (_scroll_bar != nullptr) {
		_scroll_bar->Init(this);
		_scroll_bar->ViewUpdated();
	}
}

bool _Scroll_Wnd_Impl::AddChild(Ref WndBase* child_wnd, Positioning positioning, Padding padding) {
	Ref _WndBase_Impl* child= dynamic_cast<Ref _WndBase_Impl*>(child_wnd);
	const WndStyle& style = child->GetStyle();

	Rect region_on_parent;

	if (positioning == Positioning::Fixed) {
		region_on_parent = CalculateActualRegion(style, GetSize());
		// Find the layer's proper position by z-index.
		auto it_layer = _top_layers.rbegin();
		for (; it_layer != _top_layers.rend(); ++it_layer) {
			if (style.render._z_index >= it_layer->GetZIndex()) { break; }
		}
		SingleWndLayer& layer = *_top_layers.emplace(
			it_layer.base(), this, region_on_parent, child, style.render
		);
		// Draw child window on the layer, for later compositing.
		layer.RegionUpdated(region_infinite, false);
	}

	else if (positioning == Positioning::Sticky) {
		Rect region = CalculateActualRegion(style, _base_layer.GetAccessibleRegion());
		// Find the layer's proper position by z-index.
		auto it_layer = _top_layers.rbegin();
		for (; it_layer != _top_layers.rend(); ++it_layer) {
			if (style.render._z_index >= it_layer->GetZIndex()) { break; }
		}
		SingleWndLayer& layer = *_top_layers.emplace(
			it_layer.base(), 
			this, CalculateStickyLayerRegion(region, padding), child, style.render, region, padding
		);
		// Draw child window on the layer, for later compositing.
		layer.RegionUpdated(region_infinite, false);
	}

	else {  // positioning == Positioning::Static
		Rect region = CalculateActualRegion(style, _base_layer.GetAccessibleRegion());
		_base_layer.AddWnd(child, region);
		_base_layer.RegionUpdated(region, false);  // Base layer is never redirected.
		region_on_parent = { _base_layer.ConvertToParentPoint(region.point), region.size };
	}

	_child_wnds.push_back(child);
	child->SetParent(this);

	RegionUpdated(region_on_parent);

	return true;
}

bool _Scroll_Wnd_Impl::DispatchMessage(Msg msg, Para para) {
	static Timer scroll_timer;
	static int wheel_left = 0;
	static const int wheel_per_time = 24;  // Move view in 100ms for 120px. So every 20ms 24px;
	if (msg == Msg::MouseWheel) {
		if (!scroll_timer.IsSet()) {
			scroll_timer.Set(this, 0, 20);
		}
		const MouseMsg& msg = GetMouseMsg(para);
		wheel_left += -msg.wheel_delta;  // Wheel_delta is positive when scroll away, so the view_delta is negative.
		return true;
	}
	if (msg == Msg::Timer && GetTimerMsg(para) == &scroll_timer) {
		int wheel_delta;
		if (wheel_left > 0) {
			wheel_delta = min(wheel_left, wheel_per_time);
		} else {
			wheel_delta = max(wheel_left, -wheel_per_time);
		}
		wheel_left -= wheel_delta;
		if (wheel_left == 0) { scroll_timer.Stop(); }
		MoveView({ 0, wheel_delta });
	}
	return _Wnd_Impl::DispatchMessage(msg, para);
}



END_NAMESPACE(WndDesign)
