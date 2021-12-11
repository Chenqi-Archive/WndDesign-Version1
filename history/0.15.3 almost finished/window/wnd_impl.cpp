#include "wnd_impl.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc<IWnd*> IWnd::Create(Ref<IWndBase*> parent, const WndStyle& style, Ref<ObjectBase*> object) {
	_Wnd_Impl* wnd = new _Wnd_Impl(new WndStyle(style), object);
	wnd->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(wnd); }
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
#pragma message("If new_size is empty, simply clear all child windows.")

	// Clear the base layer. Child windows will be readded later.
	_base_layer.SetCompositeSize(GetSize());
	_base_layer.ClearChild();

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
	_base_layer.RefreshCachedRegion(visible_region);
	for (auto& it : _top_layers) {
		Rect region_on_layer = visible_region.Intersect(it.GetCompositeRegion());
		region_on_layer.point = it.ConvertToLayerPoint(region_on_layer.point);
		it.RefreshCachedRegion(region_on_layer);
	}
}

void _Wnd_Impl::Composite(Rect region_to_update) const {
	FigureQueue figure_queue;

	// Draw the base layer.
	if (!_base_layer.IsRedirected()) {
		figure_queue.Push(
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
		figure_queue.Push(new LayerFigure(it, point_on_layer), intersected_region);
	}

	// Draw the border as a rectangle.
	Figure* border = new Rectangle(GetStyle().border._width, GetStyle().border._color, background_none);
	figure_queue.Push(border, Rect(point_zero, GetSize()));

	DrawFigureQueue(figure_queue, region_to_update);
}

void _Wnd_Impl::LayerUpdated(Ref<const Layer*> layer, Rect region) const {
	if (IsLayerUpdateAllowed()) {
		region.point = layer->ConvertToParentPoint(region.point);
		RegionUpdated(region);
	}
}

void _Wnd_Impl::ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) {
	_base_layer.SetResourceManager(resource_manager);
	for (auto& it : _top_layers) {
		it.SetResourceManager(resource_manager);
	}
}

bool _Wnd_Impl::ShouldAllocateNewLayer(Ref<_WndBase_Impl*> child, Rect region) {
	// If canvas is empty, don't allocate a new layer.
	if (IsEmpty()) { return false; }

	// If the region is empty, don't allocate a new layer.
	if (region.IsEmpty()) { return false; }

	// If the window has compositing effects, allocate a new layer.
	if (HasCompositeEffect(child->GetStyle())) {
		// The base layer should not be redirected.
		if (_base_layer.IsRedirected()) { _base_layer.SetUnredirected(); }
		return true; 
	}

	// If the window have a non-zero z-index, allocate a new layer.
	if (child->GetStyle().render._z_index > 0) {
		return true;
	}

	// If the window have overlapping with other bottom windows, and the window may redraw or
	//   on a top layer, allocate a new layer.
	for (const auto& it : _child_wnds) {
		if (it == child) { break; }
		if (!it->RegionOnParent().Intersect(region).IsEmpty()) {
			if (it->GetStyle().render._redraw) { return true; }
			if (it->GetLayer() != &_base_layer) { return true; }
		}
	}
	return false;
}

bool _Wnd_Impl::AddChild(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);
	const WndStyle& style = child->GetStyle();

#pragma message("If size is empty, allocate empty canvas.")

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

	_child_wnds.push_back(child);

	RegionUpdated(region);	// Composite.

	return true;
}

bool _Wnd_Impl::RemoveChild(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child) { break; } }
	if (it == _child_wnds.end()) { return false; }

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

	// Clear the focus and capture window.
	if (_capture_wnd == child) {_capture_wnd->LoseCapture();_capture_wnd = nullptr;}
	if (_focus_wnd == child) {}
	if (_last_child == child) {_last_child = nullptr;}

	_child_wnds.erase(it);

	RegionUpdated(old_region);	// Composite.

	return true;
}

bool _Wnd_Impl::UpdateChildRegion(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

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
	}

	RegionUpdated(union_region);	// Composite.
	return true;
}

void _Wnd_Impl::UpdateChildRenderStyle(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (*it == child) { break; } }
	if (it == _child_wnds.end()) { return; }

	if (child->GetLayer() == &_base_layer) {
		return;
	} else {
		// The child window has a seperate top layer, simply move the layer.
		// The layer will then call the parent window to recomposite.
		const_cast<SingleWndLayer*>(static_cast<const SingleWndLayer*>(child->GetLayer()))
			->SetStyle(child->GetStyle().render);
	}
}

bool _Wnd_Impl::DispatchMessage(Msg msg, void* para) {
	if (IsMouseMsg(msg)) {
		if (_capture_wnd == this) {
			return _WndBase_Impl::DispatchMessage(msg, para);
		}
		if (msg == Msg::MouseLeave) {
			if (_last_child != nullptr) {
				_last_child->DispatchMessage(Msg::MouseLeave, nullptr);
				_last_child = nullptr;
			}
			return _WndBase_Impl::DispatchMessage(msg, para);
		}
		// Find the child window to receive the msg.
		MouseMsg mouse_msg = GetMouseMsg(para);

		// Check the capture window.
		Ref<_WndBase_Impl*> child = _capture_wnd; 
		// Check the top windows.
		if (child == nullptr) {
			for (auto it_layer = _top_layers.rbegin(); it_layer != _top_layers.rend();++it_layer) {
				if (it_layer->GetCompositeRegion().Contains(mouse_msg.point)) {
					Point point_on_layer = it_layer->ConvertToLayerPoint(mouse_msg.point);
					child = it_layer->MouseHitTest(point_on_layer);
					if (child != nullptr) { break; }
				}
			}
		}
		// Check the windows on base layer.
		if (child == nullptr) {
			Point point_on_layer = _base_layer.ConvertToLayerPoint(mouse_msg.point);
			child = _base_layer.MouseHitTest(point_on_layer);
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
	return _WndBase_Impl::DispatchMessage(msg, para);
}



END_NAMESPACE(WndDesign)
