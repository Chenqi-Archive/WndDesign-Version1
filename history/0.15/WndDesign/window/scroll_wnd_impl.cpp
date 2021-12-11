#include "scroll_wnd_impl.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc<IScrollWnd*> IScrollWnd::Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object) {
	_Scroll_Wnd_Impl* wnd = new _Scroll_Wnd_Impl(new WndStyle(style), object, style.entire_region);
	wnd->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(wnd); }
	return wnd;
}



_Scroll_Wnd_Impl::_Scroll_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object, ScrollWndStyle::RegionStyle entire_region) :
	_Wnd_Impl(style, object, entire_region) {
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

	// Calculate sticky windows' new region.
	for (auto& it : _top_layers) {
		if (it.IsSticky()) {
			Rect region_on_parent = CalculateStickyLayerRegion(it.GetRelativeRegion(), it.GetPadding());
			if (region_on_parent != it.GetCompositeRegion()) {
				it.SetCompositeRegion(region_on_parent);
			}
		}
	}

	_is_redrawing_layer = true;

	// Send the message to scroll bar for redrawing.
	DispatchMessage(Msg::Scroll, nullptr);

	RegionUpdated(region_infinite);	// Composite.
	_is_redrawing_layer = false;
}

bool _Scroll_Wnd_Impl::AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) {
	Ref<_WndBase_Impl*> child= dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);
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
			it_layer.base(), *this, region_on_parent, child, style.render
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
			*this, CalculateStickyLayerRegion(region, padding), child, style.render, region, padding
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


END_NAMESPACE(WndDesign)
