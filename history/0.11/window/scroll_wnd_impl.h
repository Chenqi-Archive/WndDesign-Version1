#pragma once

#include "scroll_wnd.h"
#include "wnd_impl.h"
#include "../render/render_object.h"

BEGIN_NAMESPACE(WndDesign)

class ScrollRenderObject :public RenderObject{
public:
	// Allocate TileRenderTarget to the first layer.
	RenderLayer& AddLayer() override;

	void LayerUpdated(const Ref RenderLayer* layer, const Rect& rect) override;

	// Get the scroll layer to do scrolling.
	RenderLayer& GetScrollLayer() const;
};

class _Scroll_Wnd_Impl : public ScrollWnd, public _Wnd_Impl {

};


END_NAMESPACE(WndDesign)