#include "wnd_impl.h"
#include "../style/style_helper.h"
#include "../render/layer_figure.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<IWnd> IWnd::Create(Ref<IWndBase*> parent, const WndStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<IWnd> wnd = std::make_unique<_Wnd_Impl>(new WndStyle(style), object);
	if (parent != nullptr) { parent->AddChild(wnd.get()); }
	return wnd;
}



_Wnd_Impl::_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object),
	_base_layer(*this) {
}

_Wnd_Impl::_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object, ScrollWndStyle::RegionStyle entire_region) :
	_WndBase_Impl(style, object),
	_base_layer(*this, entire_region) {
}

void _Wnd_Impl::SizeChanged() {
	// Clear the base layer. Child windows will be readded later.
	_base_layer.ParentSizeChanged(GetSize());
	_base_layer.ClearChild();

#pragma message("If new_size is empty, simply clear all child windows.")


	// For all child windows, recalculate the child window' region and layer style.
	for (auto& child : _child_wnds) {
		const WndStyle& style = child->GetStyle();
		Rect new_region = child->CalculateRegionOnParent(GetSize());

		if (ShouldAllocateNewLayer(child, new_region)) {
			if (child->GetLayer() == &_base_layer) {
				// Find the layer's proper position by z-index.
				auto it_layer = _top_layers.rbegin();
				for (; it_layer != _top_layers.rend(); ++it_layer) {
					if (style.render._z_index >= it_layer->GetZIndex()) { break; }
				}
				SingleWndLayer& new_layer = *_top_layers.emplace( 
					it_layer.base(), *this, new_region, child, style.render
				);
				// Draw child window on the layer, for later compositing.
				new_layer.RegionUpdated(region_infinite, false);
			} else {
				// Just resize the layer.
			    const_cast<SingleWndLayer*>(static_cast<const SingleWndLayer*>(child->GetLayer()))
					->SetCompositeRegion(new_region);
			}
		} else {
			if (child->GetLayer() != &_base_layer) {
				// Find and delete the layer.
				for (auto it_layer = _top_layers.begin(); it_layer != _top_layers.end(); it_layer++) {
					if (&*it_layer == child->GetLayer()) { _top_layers.erase(it_layer); break; }
				}
			}
			_base_layer.AddWnd(child, new_region);
		}
	}

	_base_layer.RegionUpdated(region_infinite, false);

	// The wnd's parent layer will draw the window then, don't call RegionUpdated.
}

void _Wnd_Impl::RefreshVisibleRegion(Rect visible_region) {
	_base_layer.RefreshCachedRegion(_base_layer.ConvertToLayerRegion(visible_region));
	for (auto& it : _top_layers) {
		it.RefreshCachedRegion(it.ConvertToLayerRegion(visible_region));
	}
}

void _Wnd_Impl::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const {
	// Draw the base layer.
	if (!_base_layer.IsRedirected()) {
		figure_queue.append(
			new LayerFigure(_base_layer, _base_layer.ConvertToLayerPoint(region_to_update.point)), 
			region_to_update
		);
	} else {
		// The layer is redirected, redraw all the child windows.
		_base_layer.CompositeChild(region_to_update);
	}

	// Draw the top layers.
	for (auto& it : _top_layers) {
		Rect intersected_region = region_to_update.Intersect(it.GetCompositeRegion());
		if (intersected_region.IsEmpty()) { continue; }
		Point point_on_layer = it.ConvertToLayerPoint(intersected_region.point);
		figure_queue.append(new LayerFigure(it, point_on_layer), intersected_region);
	}

	auto& style = GetStyle();

	// Draw the border as a rectangle. Rounded rectangle may be supported later, which requires clipping.
	if (style.border._width > 0 && style.border._color != 0) {
		figure_queue.append(
			new Rectangle(style.border._width, style.border._color, background_transparent),
			Rect(point_zero, GetSize())
		);
	}
}

void _Wnd_Impl::ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) {
	_base_layer.SetResourceManager(resource_manager);
	for (auto& it : _top_layers) { it.SetResourceManager(resource_manager); }
}

bool _Wnd_Impl::ShouldAllocateNewLayer(Ref<_WndBase_Impl*> child, Rect region) {
	// If canvas is empty, don't allocate a new layer.
	if (IsEmpty()) { return false; }

	// If the region is empty, don't allocate a new layer.
	if (region.IsEmpty()) { return false; }

	auto& style = child->GetStyle();

	// If the window has compositing effects, allocate a new layer.
	if (HasCompositeEffect(style)) {
		// The base layer should not be redirected.
		if (_base_layer.IsRedirected() && !_force_base_layer_redirected) { _base_layer.SetUnredirected(); }
		return true;
	}

	// If the window have a non-zero z-index, allocate a new layer.
	if (style.render._z_index > 0) {
		return true;
	}

	// If the window have overlapping with other bottom windows, and the window may redraw or
	//   on a top layer, allocate a new layer.
	for (const auto& it : _child_wnds) {
		if (it == child) { break; }
		if (it->GetStyle().render._z_index > style.render._z_index) { continue; }
		if (!it->RegionOnParent().Intersect(region).IsEmpty()) {
			if (it->GetStyle().render._redraw) { return true; }
			if (it->GetLayer() != &_base_layer) { return true; }
		}
	}
	return false;
}

bool _Wnd_Impl::AddChild(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);
	if (const_cast<Ref<const _WndBase_Impl*>>(child)->GetParent() == this) { return true; }

	_child_wnds.emplace_back(*this, child);

	if (GetSize().IsEmpty()) {
		_base_layer.AddWnd(child, region_empty);  // child must not have nullptr layer.
		return true;
	}


	const WndStyle& style = child->GetStyle();
	// Calculate the child window's region.
	Rect region = child->CalculateRegionOnParent(GetSize());

	// Allocate a new layer or put it at the base layer.
	// The layer will provide canvas for the child window.
	if (ShouldAllocateNewLayer(child, region)) {
		// Find the layer's proper position by z-index.
		auto it_layer = _top_layers.rbegin();
		for (; it_layer != _top_layers.rend(); ++it_layer) {
			if (style.render._z_index >= it_layer->GetZIndex()) { break; }
		}
		SingleWndLayer& layer = *_top_layers.emplace(
			it_layer.base(), *this, region, child, style.render
		);
		// Draw child window on the layer, for later compositing.
		layer.RegionUpdated(region_infinite, false);
	} else {
		_base_layer.AddWnd(child, region);
		_base_layer.RegionUpdated(region, false);
	}

	UpdateRegion(region);	// Composite.

	return true;
}

bool _Wnd_Impl::RemoveChild(Ref<IWndBase*> child_wnd) {
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child_wnd) { break; } }
	if (it == _child_wnds.end()) { return false; }

	Ref<_WndBase_Impl*> child = *it;

	Rect old_region = child->RegionOnParent();

	if (child->GetLayer()== &_base_layer) {
		// The child window is on the base layer.
		_base_layer.RemoveWnd(child);
		_base_layer.RegionUpdated(_base_layer.ConvertToLayerRegion(old_region), false);
	} else {
		// The child window has a seperate top layer, find the layer and remove it.
		for (auto it_layer = _top_layers.begin(); it_layer != _top_layers.end(); it_layer++) {
			if (&*it_layer == child->GetLayer()) { _top_layers.erase(it_layer); break; }
		}
	}

	_child_wnds.erase(it);

	UpdateRegion(old_region);	// Composite.

	return true;
}

bool _Wnd_Impl::UpdateChildRegion(Ref<_WndBase_Impl*> child) {
	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child) { break; } }
	if (it == _child_wnds.end()) { return false; }
	
	// Get the old region and new region of child window. All region is relative to myself.
	Rect old_region = child->RegionOnParent();
	Rect new_region = child->CalculateRegionOnParent(GetSize());
	if (new_region == old_region) { return false; }
	Rect union_region = old_region.Union(new_region);

	if (child->GetLayer() == &_base_layer) {
		// The child window is on the base layer.
		_base_layer.MoveWnd(child, new_region);
		_base_layer.RegionUpdated(union_region, false);
	} else {
		// The child window has a seperate top layer, simply move the layer.
		// And composite all related layers.
		const_cast<SingleWndLayer*>(static_cast<const SingleWndLayer*>(child->GetLayer()))
			->SetCompositeRegion(new_region);
		// SingleWndLayer has updated the entire region.
	}

	UpdateRegion(union_region);	// Composite.
	return true;
}

void _Wnd_Impl::UpdateChildRenderStyle(Ref<_WndBase_Impl*> child) {
	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child) { break; } }
	if (it == _child_wnds.end()) { return; }
	
	if (child->GetLayer() == &_base_layer) {
		return;
	} else {
		SingleWndLayer& layer = static_cast<SingleWndLayer&>(*child->GetLayer());
		layer.SetStyle(child->GetStyle().render);
		UpdateRegion(layer.GetCompositeRegion());
	}
}

Ref<_WndBase_Impl*> _Wnd_Impl::HitTestChild(Point point) const {
	Ref<_WndBase_Impl*> child = nullptr;

	// Hit-test the top windows.
	for (auto it_layer = _top_layers.rbegin(); it_layer != _top_layers.rend(); ++it_layer) {
		if (it_layer->GetCompositeRegion().Contains(point)) {
			child = it_layer->HitTestChild(it_layer->ConvertToLayerPoint(point));
			if (child != nullptr) { return child; }
		}
	}

	// Hit-test windows on base layer.
	child = _base_layer.HitTestChild(_base_layer.ConvertToLayerPoint(point));
	return child;
}


END_NAMESPACE(WndDesign)