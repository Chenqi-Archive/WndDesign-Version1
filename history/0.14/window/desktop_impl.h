#pragma once

#include "desktop.h"
#include "wnd_base_impl.h"
#include "../render/system_layer.h"


BEGIN_NAMESPACE(WndDesign)


class SystemWndLayer;

class _Desktop_Impl final : public Desktop , public _WndBase_Impl{
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:  // Forbid construction.
	_Desktop_Impl();
	~_Desktop_Impl() {}	// Message loop will automatically quit after destroying all layers.

public:
	static Ref _Desktop_Impl* Get();


	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
public:
	void Composite(Rect region_to_update) const override {}
	void RefreshVisibleRegion(Rect visible_region) override {}

	// Draw the layer to the system window.
	void LayerUpdated(const Ref Layer* layer, Rect region) const override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<SystemWndLayer> _layers;
public:
	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;
	void MoveChild(Ref WndBase* child_wnd) override;
	void SetChildRenderStyle(Ref WndBase* child_wnd) override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	int EnterMessageLoop(); // Enter the system message loop.
	void SetCapture(_WndBase_Impl* child) override;
	void ReleaseCapture() override;

private:
	enum : uint { _clean_target_interval = 60000 };  // 1 min
	Timer _timer_clean_target;
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)