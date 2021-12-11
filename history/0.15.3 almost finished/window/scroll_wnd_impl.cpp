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

void _Scroll_Wnd_Impl::SizeChanged() {
	BlockLayerUpdate();

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
				Rect accessible_region = _base_layer.GetAccessibleRegion();
				Rect relative_region = it.GetWnd()->CalculateRegionOnParent(accessible_region.size) + (accessible_region.point - point_zero);
				it.SetRelativeRegion(relative_region);
			}
			Rect new_region = CalculateStickyLayerRegion(it.GetRelativeRegion(), it.GetPadding());
			if (new_region != it.GetCompositeRegion()) { it.SetCompositeRegion(new_region); }
		} else {
			Rect new_region = it.GetWnd()->CalculateRegionOnParent(GetSize());
			it.SetCompositeRegion(new_region);
		}
	}

	// Send the message to scroll bar for redrawing.
	DispatchMessage(Msg::Scroll, nullptr);
	AllowLayerUpdate();
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

	BlockLayerUpdate();

	// Calculate sticky windows' new region.
	for (auto& it : _top_layers) {
		if (it.IsSticky()) {
			Rect region_on_parent = CalculateStickyLayerRegion(it.GetRelativeRegion(), it.GetPadding());
			if (region_on_parent != it.GetCompositeRegion()) {
				it.SetCompositeRegion(region_on_parent);
			}
		}
	}

	// Send the message to scroll bar for redrawing.
	DispatchMessage(Msg::Scroll, nullptr);

	RegionUpdated(region_infinite);	// Composite.
	AllowLayerUpdate();
}

bool _Scroll_Wnd_Impl::AddChild(Ref<IWndBase*> child_wnd, Positioning positioning, Padding padding) {
	Ref<_WndBase_Impl*> child= dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);
	const WndStyle& style = child->GetStyle();

	Rect region_on_parent;

	if (positioning == Positioning::Fixed) {
		region_on_parent = child->CalculateRegionOnParent(GetSize());

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
		Rect accessible_region = _base_layer.GetAccessibleRegion();
		Rect region = child->CalculateRegionOnParent(accessible_region.size) + (accessible_region.point - point_zero);

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
		Rect accessible_region = _base_layer.GetAccessibleRegion();
		Rect region = child->CalculateRegionOnParent(accessible_region.size) + (accessible_region.point - point_zero);

		_base_layer.AddWnd(child, region);
		_base_layer.RegionUpdated(region, false);  // Base layer is never redirected.
		region_on_parent = _base_layer.ConvertToParentRegion(region);
	}

	_child_wnds.push_back(child);

	RegionUpdated(region_on_parent);

	return true;
}

bool _Scroll_Wnd_Impl::UpdateChildRegion(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child) { break; } }
	if (it == _child_wnds.end()) { return false; }

	Rect region_updated;

	Ref<Layer*> layer = child->GetLayer();
	if (layer == &_base_layer) {
		// Get the old region and new region of child window. All region is relative to the base layer.
		Rect old_region = child->RegionOnLayer();
		Rect accessible_region = _base_layer.GetAccessibleRegion();
		Rect new_region = child->CalculateRegionOnParent(accessible_region.size) + (accessible_region.point - point_zero);
		if (new_region == old_region) { return false; }
		_base_layer.MoveWnd(child, new_region);
		Rect union_region = old_region.Union(new_region);
		_base_layer.RegionUpdated(union_region, false);
		region_updated = _base_layer.ConvertToParentRegion(union_region);
	} else {
		Ref<SingleWndLayer*> top_layer = static_cast<SingleWndLayer*>(layer);
		Rect old_region = top_layer->GetCompositeRegion();
		Rect new_region;
		if (top_layer->IsSticky()) {
			// Recalculate the relative region of sticky layer.
			Rect accessible_region = _base_layer.GetAccessibleRegion();
			Rect relative_region = child->CalculateRegionOnParent(accessible_region.size) + (accessible_region.point - point_zero);
			top_layer->SetRelativeRegion(relative_region);
			new_region = CalculateStickyLayerRegion(top_layer->GetRelativeRegion(), top_layer->GetPadding());
		} else {
			new_region = child->CalculateRegionOnParent(GetSize());
		}
		if (new_region == old_region) { return false; }
		top_layer->SetCompositeRegion(new_region);
		region_updated = old_region.Union(new_region);
	}

	RegionUpdated(region_updated);	// Composite.
	return true;
}


END_NAMESPACE(WndDesign)