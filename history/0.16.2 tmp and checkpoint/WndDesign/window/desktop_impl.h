#pragma once

#include "desktop.h"
#include "wnd_base_impl.h"
#include "../render/system_layer.h"


BEGIN_NAMESPACE(WndDesign)


class SystemWndLayer;

class _Desktop_Impl final : public IDesktop , public _WndBase_Impl{
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:  // Forbid construction.
	_Desktop_Impl();
	~_Desktop_Impl() {}	// Message loop will automatically quit after destroying all layers.

public:
	static _Desktop_Impl& Get();

	void Destroy() override {  // Destroy all child windows, and exit the message loop.
		_layers.clear();
	}


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<ChildWndContainer> _child_wnds;
public:
	bool AddChild(Ref<IWndBase*> child_wnd) override;
	bool RemoveChild(Ref<IWndBase*> child_wnd) override;
	bool UpdateChildRegion(Ref<IWndBase*> child_wnd) override;
	void UpdateChildRenderStyle(Ref<IWndBase*> child_wnd) override;


	///////////////////////////////////////////////////////////
	////                     Composite                     ////
	///////////////////////////////////////////////////////////
private:
	list<SystemWndLayer> _layers;
public:
	void Composite(Rect region_to_update) const override {}
	void RefreshVisibleRegion(Rect visible_region) override {}


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
public:
	int MessageLoop() override;
	void SetCapture(_WndBase_Impl* child) override;
	void ReleaseCapture() override;

private:
	// For cleaning free target on a regular basis.
	static const uint clean_target_interval = 60000;  // 1 min
	static void CleanTimerProc(Handle timer, void* para);
	Handle _timer;
	void CleanUnusedTargets() {
		for (auto& it : _layers) { it.CleanUnusedTargets(); }
	}
};


END_NAMESPACE(WndDesign)