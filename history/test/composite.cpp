	void Composite(Rect region_updated = region_infinite) const override {
		region_updated = region_updated.Intersect({ point_zero, _canvas.GetSize() });
		FigureQueue figure_queue;
		// Draw base layer.
		if (!_base_layer.IsRedirected()) {
			figure_queue.Push(new LayerFigure(_base_layer, region_updated.point), region_updated);
		}
		// Draw top layers.
		for (auto layer : _top_layers) {
			Rect intersected_region = region_updated.Intersect(layer.GetCompositeRegion());
			if (intersected_region.IsEmpty()) { 
				continue; 
			}
			Point point_on_layer = layer.ConvertToLayerPoint(intersected_region.point);
			figure_queue.Push(new LayerFigure(layer, point_on_layer), intersected_region);
		}
		_canvas.DrawFigureQueue(figure_queue, vector_zero, region_updated);
	}

	void Composite(const Ref Layer* layer, Rect updated_region_on_layer) {
		FigureQueue figure_queue;

		// Calculate the region to update on the window.
		Rect region_updated = updated_region_on_layer;
		region_updated.point = layer->ConvertToParentPoint(region_updated.point);
		region_updated.Intersect({ point_zero, _canvas.GetSize() });
		if (region_updated.IsEmpty()) {
			return;
		}

		auto it_layer = _top_layers.begin();
		if (layer != &_base_layer) {
			if (!layer->IsOpaque()) {
				// If the layer might be transparent, redraw the lower layers.
				if (!_base_layer.IsRedirected()) {
					// The base layer will never be redirected.
					figure_queue.Push(new LayerFigure(_base_layer, region_updated.point), region_updated);
				}
				for (; it_layer != _top_layers.end(); ++it_layer) {
					if (&*it_layer == layer) { break; }
					Rect intersected_region = region_updated.Intersect(it_layer->GetCompositeRegion());
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
				// Layer not found. The figure queue will destroy automatically.
				return;
			}
			Rect intersected_region = region_updated.Intersect(it_layer->GetCompositeRegion());
			Point point_on_layer = it_layer->ConvertToLayerPoint(region_updated.point);
			figure_queue.Push(new LayerFigure(*it_layer, point_on_layer), intersected_region);
		} else {
			figure_queue.Push(new LayerFigure(_base_layer, region_updated.point), region_updated);
		}

		// Draw the top layers.
		for (; it_layer != _top_layers.end(); ++it_layer) {
			Rect intersected_region = region_updated.Intersect(it_layer->GetCompositeRegion());
			if (intersected_region.IsEmpty()) { continue; }
			Point point_on_layer = it_layer->ConvertToLayerPoint(intersected_region.point);
			figure_queue.Push(new LayerFigure(*it_layer, point_on_layer), intersected_region);
		}

		_canvas.DrawFigureQueue(figure_queue, vector_zero, region_updated);
	}
