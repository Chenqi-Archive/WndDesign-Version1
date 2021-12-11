#pragma once

#include "layer.h"

#include "../message/msg.h"
#include "../system/metrics.h"

#include "../render/direct2d/target.h"

BEGIN_NAMESPACE(WndDesign)

class SystemWndLayer : public SingleWndLayer {
private:
	TargetResourceManager _target_resource_manager;
	mutable Tile _system_target_tile;

public:
	SystemWndLayer(_WndBase_Impl& parent, Rect region, Ref<_WndBase_Impl*> wnd,
				   WndStyle::RenderStyle style);
	~SystemWndLayer();

	void CleanUnusedTargets() { _target_resource_manager.CleanUnusedTargets(); }

	void SetRegion(Rect region);

	// For a system layer, the entire region should be visible.
	Rect GetLayerVisibleRegion() const override { return GetAccessibleRegion(); }

	void DrawRegion(Rect region_on_layer) const {
		_system_target_tile.BeginDraw(region_on_layer);
		DrawOn(region_on_layer.point, _system_target_tile, region_on_layer);
		_system_target_tile.EndDraw();
	}

	void ReceiveSystemMessage(Msg msg, void* para) const;

	void SetCapture();
	void ReleaseCapture();
	void LoseCapture();

	void SetFocus();
	void ReleaseFocus();
	void LoseFocus();
};

END_NAMESPACE(WndDesign)