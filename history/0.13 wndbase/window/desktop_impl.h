#pragma once

#include "desktop.h"

#include "wnd_impl.h"
#include "../render/layer.h"
#include "../system/metrics.h"

#include "../render/direct2d/direct2d.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::list;

class SystemWndLayer : public SingleWndLayer {
private:
	TargetResourceManager _target_resource_manager;
	mutable Tile _system_target_tile;

	Ref TargetResourceManager* AcquireResourceManager() override {
		return &_target_resource_manager;
	}

public:
	SystemWndLayer();
	~SystemWndLayer();
	
	void Init(Ref _WndBase_Impl* parent, Rect region, Ref _WndBase_Impl* wnd,
			  WndStyle::RenderStyle style);

	void Clear() override;

	void SetRegion(Rect region);

	void RegionUpdated(Rect region_on_layer) const {
		_system_target_tile.BeginDraw(region_on_layer);
		DrawOn(region_on_layer.point, _system_target_tile, region_on_layer);
		_system_target_tile.EndDraw();
	}

	void ReceiveSystemMessage(Msg msg, void* para) const {
		_wnd->DispatchMessage(msg, para);
	}
};


class _Desktop_Impl final : public Desktop , public _Wnd_Impl{
private: 
	// Disgarded functions.
	void SetHandler(Handler handler) override {}
	void SetSize(LengthTag width, LengthTag height) override {}
	void SetPosition(LengthTag left, LengthTag top) override {}
	void SetBackground(Color32 background) override {}
	void SetOpacity(uchar opacity) override {}
	Point ConvertToParentPoint(Point point) override { return point_zero; }
	void SetUserData(Data data) override {}
	Data GetUserData() const override { return 0; }

	void SetCanvas(const Canvas& canvas) override {}
	void RefreshVisibleRegion() override {}
	bool DispatchMessage(Msg msg, void* para) override { return false; }


	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Desktop_Impl();
	~_Desktop_Impl() override;

	void Destroy() override;


	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
public:
	// Draw the layer to the system window.
	void Composite(Rect region_to_update = region_infinite, const Ref Layer* layer = nullptr) const override;
	void LayerUpdated(const Ref Layer* layer, Rect region) override;

	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<SystemWndLayer> _layers;

public:
	// Called by child layers, for querying the visible region.
	// Always returns the whole region, and the layers' visible region will also be the whole region.
	Rect GetVisibleRegion() const override { return { point_zero,GetDesktopSize() }; }

	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;
	void MoveChild(Ref WndBase* child_wnd) override;
	void SetChildRenderStyle(Ref WndBase* child_wnd) override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:

public:
	// Enter the system message loop.
	int EnterMessageLoop();

	// Add a new dialog window and block the message input to parent window.
	void AddModalDialogBox(const Ref WndBase* dialog, const Ref WndBase* parent) override {

	}
};


Ref _Desktop_Impl* GetDesktop();


END_NAMESPACE(WndDesign)