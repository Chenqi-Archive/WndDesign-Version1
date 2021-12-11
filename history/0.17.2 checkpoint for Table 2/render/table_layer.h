#pragma once

#include "layer.h"


BEGIN_NAMESPACE(WndDesign)


class _Table_Impl;


// Table layer is the base layer of Table to accommodate all child windows.
// The layer is redirected by default, and can be redirected if scrolling is supported.
class TableLayer : public Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
public:
	TableLayer(_Table_Impl& parent);
	~TableLayer() {}

private:
	_Table_Impl& GetParent() const { return static_cast<_Table_Impl&>(_parent); }


	// Functions below are only called by parent windows. Layer itself should never call these functions.
	// Similar to MultipleWndLayer.
public:
	void SetUnredirected() {
		if (!_is_redirected) { return; }
		_is_redirected = false;
		RegionUpdated(region_infinite, false);  // Now tile cache will receive the first figure queue.
	}

	// Set the size of composite region when parent Table's size has changed.
	// Composite region is always the whole parent region.
	void SetCompositeSize(Size parent_size) {
		_composite_region.size = parent_size;  // Point has been initialized to zero.
	}

	// Set size when resizing the table or inserting or removing columns/rows or change child windows.
	void SetLayerSize(Size size);

	// Set the current point when scrolling.
	// Return true if current point has changed.
	bool SetCurrentPoint(Point point) {
		point = BoundPointInRegion(point, ShrinkRegionBySize(GetAccessibleRegion(), GetCompositeRegion().size));
		if (point == GetCurrentPoint()) { return false; }
		_current_point = point;
		RefreshCachedRegion(GetLayerVisibleRegion());
		return true;
	}


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
public:
	// Called by parent Table to set canvas for child windows.
	void SetChildCanvas(_WndBase_Impl& child, Rect region) {
		child.SetCanvas(Canvas(this, region));
	}

private:
	// Only called by RefreshCachedRegion() when the cached region has changed.
	void RefreshChildVisibleRegion(Rect cached_region) const override;
	// Only called by RegionUpdated() and CompositeChild().
	void RedrawChild(Rect region_to_update) const override;

	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) override;

private:
	// Never called, parent Table will do this by itself. 
	Ref<_WndBase_Impl*> MouseHitTest(Point point) const override { return nullptr; }
};


END_NAMESPACE(WndDesign)