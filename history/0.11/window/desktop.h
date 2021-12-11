#pragma once

#include "../render/render_object.h"

BEGIN_NAMESPACE(WndDesign)

class _Wnd_Impl;

// Desktop is a render object that has multiple layers 
//   with one sub window on each layer.
class _Desktop : RenderObject{

public:
	// Allocate HWNDRenderTarget to sub layers.
	RenderLayer& AddLayer() override;

	void LayerUpdated(const Ref RenderLayer* layer, const Rect& rect) override;
};


END_NAMESPACE(WndDesign)
