#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)

WNDDESIGN_API Alloc Wnd* Wnd::Create(Ref Wnd* parent, const WndStyle& style, Handler handler) {
	Wnd* wnd = new _Wnd_Impl(dynamic_cast<_WndBase_Impl*>(parent), style, handler);
	if (parent != nullptr) { parent->AddChild(wnd); }
	return wnd;
}



_Wnd_Impl::_Wnd_Impl(Ref _WndBase_Impl* parent, const WndStyle& style, Handler handler) :
	_WndBase_Impl(parent, style, handler) {
}

_Wnd_Impl::~_Wnd_Impl() {}

void _Wnd_Impl::SetCanvas(const Canvas& canvas) {
	bool size_changed = (_canvas.GetSize() != canvas.GetSize());
	_canvas = canvas;

	// Block the composition request issued by any layer.
	_is_redrawing_layer = true;

	if (size_changed) {
		// Clear the base layer. Child windows will be added later.
		_base_layer.Init(this, _canvas.GetSize(), true);

		// The current top layer position to insert new layer.
		auto it_layer = _top_layers.begin();

		// For all child windows, recalculate the child window' region and layer style.
		for (auto& it : _child_wnds) {
			Ref _WndBase_Impl* child = it.wnd;
			const WndStyle& style = child->GetStyle();
			Rect new_region = style.CalculateActualRegion(_canvas.GetSize());
			it.region = new_region;

			if (ShouldAllocateNewLayer(child, new_region)) {
				if (it.layer == &_base_layer) {
					// Create a new layer and insert it to the current position.
					auto new_layer = _top_layers.insert(it_layer, SingleWndLayer());
					new_layer->Init(this, new_region, child, style.render);
					it.layer = &*new_layer;
				} else {
					if (&*it_layer != it.layer) { BreakPoint(); } // Layer does not match!
					// Just resize the layer.
					static_cast<SingleWndLayer*>(it.layer)->SetCompositeRegion(new_region);
				}
			} else {
				if (it.layer != &_base_layer) {
					// Delete the layer
					if (&*it_layer != it.layer) { BreakPoint(); } // Layer does not match!
					_top_layers.erase(it_layer++);
				}
				_base_layer.AddWnd(child, new_region);
			}
		}
	}

	Composite();
	_is_redrawing_layer = false;
}

void _Wnd_Impl::RefreshVisibleRegion() {
	Rect visible_region = GetVisibleRegion();
	_is_redrawing_layer = true;
	_base_layer.RefreshVisibleRegion(visible_region);
	for (auto& it : _top_layers) {
		it.RefreshVisibleRegion(visible_region);
	}
	Composite(visible_region);
	_is_redrawing_layer = false;
}

void _Wnd_Impl::Composite(Rect region_to_update, const Ref Layer* layer) const {
	// If layer is a top layer, convert the region so as to relative to the parent window.
	if (layer != &_base_layer && layer != nullptr) {
		region_to_update.point = layer->ConvertToParentPoint(region_to_update.point);
	}
	region_to_update = region_to_update.Intersect({ point_zero, _canvas.GetSize() });
	if (region_to_update.IsEmpty()) { return; }

	FigureQueue figure_queue;
	auto it_layer = _top_layers.begin();

	// Case 1: The layer is a top layer, if not opaque, redraw all the overlapped layers, 
	//           else simply find the layer, and draw the layer itself.
	if (layer != &_base_layer && layer != nullptr) {
		// If the layer might be transparent, redraw the lower layers.
		if (!layer->IsOpaque()) {
			if (!_base_layer.IsRedirected()) {
				figure_queue.Push(new LayerFigure(_base_layer, region_to_update.point), region_to_update);
			} else {
				BreakPoint(); // Never reaches here. The base layer will never be redirected.
				// The layer is redirected, redraw all the child windows.
				_base_layer.RefreshChildVisibleRegion();
			}
			for (; it_layer != _top_layers.end(); ++it_layer) {
				if (&*it_layer == layer) { break; }
				Rect intersected_region = region_to_update.Intersect(it_layer->GetCompositeRegion());
				if (intersected_region.IsEmpty()) { continue; }
				Point point_on_layer = it_layer->ConvertToLayerPoint(intersected_region.point);
				figure_queue.Push(new LayerFigure(*it_layer, point_on_layer), intersected_region);
			}
		} else {
			// 3. Else find the layer directly.
			for (; it_layer != _top_layers.end(); ++it_layer) { if (&*it_layer == layer) { break; } }
		}

		// Draw the layer itself.
		if (it_layer == _top_layers.end()) {
			return;  // Layer not found. The figure queue will destroy automatically.
		}
		Rect intersected_region = region_to_update.Intersect(it_layer->GetCompositeRegion());
		Point point_on_layer = it_layer->ConvertToLayerPoint(region_to_update.point);
		figure_queue.Push(new LayerFigure(*it_layer, point_on_layer), intersected_region);
	} 
	// Case 2: The layer is the base layer, if not redirected, draw itself, else do not draw anything.
	else if (layer == &_base_layer){
		if (!_base_layer.IsRedirected()) {
			figure_queue.Push(new LayerFigure(_base_layer, region_to_update.point), region_to_update);
		} 
	}
	// Case 3: The layer is nullptr, redraw the base layer.
	else {
		if (!_base_layer.IsRedirected()) {
			figure_queue.Push(new LayerFigure(_base_layer, region_to_update.point), region_to_update);
		} else {
			// The layer is redirected, redraw all the child windows.
			_base_layer.RefreshChildVisibleRegion();
		}
	}

	// Draw the top layers.
	for (; it_layer != _top_layers.end(); ++it_layer) {
		Rect intersected_region = region_to_update.Intersect(it_layer->GetCompositeRegion());
		if (intersected_region.IsEmpty()) { continue; }
		Point point_on_layer = it_layer->ConvertToLayerPoint(intersected_region.point);
		figure_queue.Push(new LayerFigure(*it_layer, point_on_layer), intersected_region);
	}

	_canvas.DrawFigureQueue(figure_queue, vector_zero, region_to_update);
}

void _Wnd_Impl::LayerUpdated(const Ref Layer* layer, Rect region) {
	if (!_is_redrawing_layer) {
		Composite(region, layer);
	}
}

bool _Wnd_Impl::ShouldAllocateNewLayer(Ref _WndBase_Impl* child, Rect region) {
	// If the region is empty, don't allocate a new layer.
	if (region.IsEmpty()) { return false; }

	// If the window has compositing effects, allocate a new layer.
	if (child->GetStyle().ShouldAllocateNewLayer()) { 
		// The base layer should not be redirected.
		if (_base_layer.IsRedirected()) {
			_base_layer.SetUnredirected();
		}
		return true; 
	}

	// If the window have overlapping with other bottom windows, and the window may redraw or
	//   on a top layer, allocate a new layer.
	for (const auto& it : _child_wnds) {
		if (it.wnd == child) { break; }
		if (!it.region.Intersect(region).IsEmpty()) {
			if (it.wnd->GetStyle().MayRedraw()) { return true; }
			if (it.layer != &_base_layer) { return true; }
		}
	}
	return false;
}

bool _Wnd_Impl::AddChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);
	const WndStyle& style = child->GetStyle();
	// Calculate the child window's actual region.
	Rect region;
	if (_canvas.IsEmpty()) {
		region = region_empty;
	} else {
		region = style.CalculateActualRegion(_canvas.GetSize());
	}
	// Allocate a new layer or put it at the base layer.
	// The layer will provide canvas for the child window.
	Ref Layer* pLayer;
	if (ShouldAllocateNewLayer(child, region)) {
		// The new layer is a single window layer, whose composite region is just the child window's region.
		_top_layers.push_back(SingleWndLayer()); SingleWndLayer& layer = _top_layers.back();
		layer.Init(this, region, child, style.render);
		pLayer = &layer;
	} else {
		_base_layer.AddWnd(child, region);
		pLayer = &_base_layer;
	}
	_child_wnds.push_back({ child, region, pLayer });
	return true;
}

bool _Wnd_Impl::RemoveChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
	if (it == _child_wnds.end()) { return false; }

	// Get the child window's layer.And recomposite immediately.
	_is_redrawing_layer = true;
	Ref Layer* pLayer = it->layer;
	if (pLayer == &_base_layer) {
		// The child window is on the base layer.
		_base_layer.RemoveWnd(child);
	} else {
		// The child window has a seperate top layer, find the layer and remove it.
		for (auto it_layer = _top_layers.begin(); it_layer != _top_layers.end(); ++it_layer) {
			if (pLayer == &*it_layer) {
				_top_layers.erase(it_layer);
				break;
			}
		}
	}
	Composite(it->region, nullptr);
	_is_redrawing_layer = false;

	_child_wnds.erase(it);

	return true;
}


// The child window's position or size has changed.
void _Wnd_Impl::MoveChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
	if (it == _child_wnds.end()) { return; }

	// Calculate the new region.
	const WndStyle& style = child->GetStyle();
	Rect new_region = style.CalculateActualRegion(_canvas.GetSize());
	if (new_region == it->region) { return; }

	Rect union_region = new_region.Union(it->region);
	it->region = new_region;

	if (it->layer == &_base_layer) {
		// The child window is on the base layer.
		// The base layer will redraw and automatically call the parent to composite.
		_base_layer.MoveWnd(child, new_region);
	} else {
		_is_redrawing_layer = true;
		// The child window has a seperate top layer, simply move the layer.
		// And composite all related layers.
		static_cast<SingleWndLayer*>(it->layer)->SetCompositeRegion(new_region);
		Composite(union_region, nullptr);
		_is_redrawing_layer = false;
	}
}

void _Wnd_Impl::SetChildRenderStyle(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	// Find the child window.
	auto it = _child_wnds.begin();
	for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
	if (it == _child_wnds.end()) { return; }

	// Get the child window's layer.
	Ref Layer* pLayer = it->layer;
	if (pLayer == &_base_layer) {
		return;
	} else {
		// The child window has a seperate top layer, simply move the layer.
		// The layer will then call the parent window to recomposite.
		pLayer->SetStyle(child->GetStyle().render);
	}
}

bool _Wnd_Impl::DispatchMessage(Msg msg, void* para) {
	if (IsMouseMsg(msg)) {
		MouseMsg mouse_msg = GetMouseMsg(para);
		// Find the child window to receive the msg.
		Ref _WndBase_Impl* child = nullptr;
		for (const auto& it_layer : _top_layers) {
			if (it_layer.GetCompositeRegion().Contains(mouse_msg.point)) {
				child = it_layer.MsgHitTest(it_layer.ConvertToLayerPoint(mouse_msg.point));
				if (child != nullptr) { break; }
			}
		}
		if (child == nullptr) {
			child = _base_layer.MsgHitTest(mouse_msg.point);
		}
		// Deliver the message to child window.
		if (child != nullptr) {
			// Find the child window.
			auto it = _child_wnds.begin();
			for (; it != _child_wnds.end(); ++it) { if (it->wnd == child) { break; } }
			if (it == _child_wnds.end()) { BreakPoint(); return false; }
			// Convert to point on child window.
			mouse_msg.point = mouse_msg.point - (it->region.point - point_zero);
			return child->DispatchMessage(msg, &mouse_msg);
		}
	}

	if (_handler != nullptr) {
		return _handler(this, msg, para);
	}
	return false;
}



END_NAMESPACE(WndDesign)
