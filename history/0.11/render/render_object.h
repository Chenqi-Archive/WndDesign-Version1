#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "render_style.h"
#include "render_layer.h"
#include "render_target.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)


class RenderObject {
private:
	// The wnd.
	// Ref _Wnd_Impl* wnd;  // No need.

	// The render target to draw on.
	RenderTarget _target;

	// The sub render layers that hold all sub windows.
	// The first layer has no effect style.
	std::list<RenderLayer> _sub_layers;


public:
	virtual RenderLayer& AddLayer();
	virtual void LayerUpdated(const Ref RenderLayer* layer, const Rect& rect);
};



END_NAMESPACE(WndDesign)