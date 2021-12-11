#pragma once

#include "wnd.h"
#include "wnd_base_impl.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;


class _Wnd_Impl : virtual public IWnd, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object);
	// Used by derived class ScrollWnd to initialize base layer.
	_Wnd_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object, ScrollWndStyle::RegionStyle entire_region);
	~_Wnd_Impl() {}


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
protected:
	list<ChildContainer> _child_wnds;

	// Called when adding a new child window or recompute canvas.
	bool ShouldAllocateNewLayer(Ref<_WndBase_Impl*> child, Rect region);

public:
	bool AddChild(Ref<IWndBase*> child_wnd) override;
	bool RemoveChild(Ref<IWndBase*> child_wnd) override;
	bool UpdateChildRegion(Ref<_WndBase_Impl*> child) override;
	void UpdateChildRenderStyle(Ref<_WndBase_Impl*> child) override;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
protected:
	MultipleWndLayer _base_layer;
	list<SingleWndLayer> _top_layers;

public:
	// The wnd's layer has refreshed, refresh the visible region for child layers.
	void RefreshVisibleRegion(Rect visible_region) override;
	// Composite all layers on the region to update.
	void Composite(Rect region_to_update) const override;

private:
	void ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) override;
	// Recalculate layer regions and reset canvas for child windows.
	void SizeChanged() override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)