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
	_Wnd_Impl(Ref _WndBase_Impl* parent, const WndStyle& style, Handler handler);
	~_Wnd_Impl() override;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
protected:
	MultipleWndLayer _base_layer;
	list<SingleWndLayer> _top_layers;

public:
	// Set the canvas, allocate canvases for child windows, and composite.
	void SetCanvas(const Canvas& canvas) override;

	// The wnd's own layer has refreshed, refresh the visible region for child layers,
	//   and then recomposite.
	void RefreshVisibleRegion() override;

	// Composite all layers on the region to update.
	// If layer is nullptr, the region is relative to the parent window, else relative to the layer.
	void Composite(Rect region_to_update = region_infinite, const Ref Layer* layer = nullptr) const override;

	// The region on the layer has updated, the window has to recomposite.
	// The composite operation may be masked or delayed.
	void LayerUpdated(const Ref Layer* layer, Rect region) override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
protected:
	struct ChildWndInfo {
		Ref _WndBase_Impl* wnd;
		Rect region;
		Ref Layer* layer;
	};
	list<ChildWndInfo> _child_wnds;

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