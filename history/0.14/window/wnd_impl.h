#pragma once

#include "wnd.h"
#include "wnd_base_impl.h"

#include "../render/layer_figure.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::list;

class _Wnd_Impl : virtual public Wnd, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Wnd_Impl(Alloc WndStyle* style, Handler handler);
	// Used by derived class ScrollWnd to initialize base layer.
	_Wnd_Impl(Alloc WndStyle* style, Handler handler, ScrollWndStyle::RegionStyle entire_region);
	~_Wnd_Impl() {}


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
protected:
	MultipleWndLayer _base_layer;
	list<SingleWndLayer> _top_layers;

public:
	// Set the canvas, allocate canvases for child windows, and composite.
	void SetCanvas(const Canvas& canvas) override;
	// The wnd's layer has refreshed, refresh the visible region for child layers.
	void RefreshVisibleRegion(Rect visible_region) override;
	// Composite all layers on the region to update.
	void Composite(Rect region_to_update) const override;
	// The region on the layer has updated, the window has to recomposite.
	// The composite operation may be masked or delayed.
	void LayerUpdated(const Ref Layer* layer, Rect region) const override;

private:
	void ResetLayerResourceManager(Ref TargetResourceManager* resource_manager) override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
protected:
	list<Ref _WndBase_Impl*> _child_wnds;

	// Called when adding a new child window or recompute canvas.
	bool ShouldAllocateNewLayer(Ref _WndBase_Impl* child, Rect region);

public:
	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;
	void MoveChild(Ref WndBase* child_wnd) override;
	void SetChildRenderStyle(Ref WndBase* child_wnd) override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)