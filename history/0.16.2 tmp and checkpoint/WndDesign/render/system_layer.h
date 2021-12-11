#pragma once

#include "layer.h"

#include "../message/msg.h"
#include "../system/metrics.h"

#include "../render/direct2d/target.h"


BEGIN_NAMESPACE(WndDesign)


class SystemWndLayer : public SingleWndLayer {
private:
	TargetResourceManager _target_resource_manager;
	Tile _system_target_tile;

public:
	SystemWndLayer(_WndBase_Impl& parent, Rect region, Ref<_WndBase_Impl*> wnd,
				   WndStyle::RenderStyle style);
	~SystemWndLayer();

	void CleanUnusedTargets() { _target_resource_manager.CleanUnusedTargets(); }

	void SetRegion(Rect region);

	// For a system layer, the entire region should be visible.
	Rect GetLayerVisibleRegion() const override { return GetAccessibleRegion(); }

	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) override;

	void ReceiveSystemMessage(Msg msg, void* para) const;

	void SetCapture();
	void ReleaseCapture();
	void LoseCapture();

	void SetFocus();
	void ReleaseFocus();
	void LoseFocus();
};

END_NAMESPACE(WndDesign)