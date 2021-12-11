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

public:
	// For a system layer, the entire region should be visible.
	Rect GetLayerVisibleRegion() const override { return GetAccessibleRegion(); }
	void RedrawChild(Rect region_to_update) const override;
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) override;
	bool IsValid() const;  // When the layer is being destroyed, it becomes invalid and no longer receives system messages.

public:
	// Called by desktop.
	void CleanUnusedTargets() { _target_resource_manager.CleanUnusedTargets(); }

	void SetRegion(Rect region);
	void StyleChanged();

	void SetCapture();
	void ReleaseCapture();

	void SetFocus();
	void ReleaseFocus();

	// Called by WndProc(). (plus RegionUpdated() for WM_PAINT)
	void RegionChanged(Rect region);
	void ReceiveSystemMessage(Msg msg, void* para) const;
	void LoseCapture();
	void LoseFocus();

	std::pair<Size, Size> CalculateMinMaxSize();
};


END_NAMESPACE(WndDesign)