#pragma once

#include "layer.h"

#include "../common/exception.h"


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
	_Table_Impl& GetParent() const;


	// Functions below are only called by parent windows. Layer itself should never call these functions.
	// Similar to MultipleWndLayer.
public:
	void SetUnredirected() {
		if (!_is_redirected) { Error(); }
		_is_redirected = false;
		RegionUpdated(region_infinite, false);  // Now tile cache will receive the first figure queue.
	}

	// Set the size of composite region when parent Table's size has changed.
	// Composite region is always the whole parent region.
	void ParentSizeChanged(Size parent_size) {
		_composite_region.size = parent_size;  // Point has been initialized to zero.
		// No need to reset accessible region as MultipleWndLayer. 
		// The accessible region is only dependent on grid contents, it will be reset at SetLayerSize().
	}

	// Set the width of accessible region when the column position changed.
	bool SetWidth(uint new_width);
	bool SetHeight(uint new_height);
	

	// Set the current point when scrolling.
	// Return true if current point has changed.
	bool SetCurrentPoint(Point point);


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
private:
	// Only called by RefreshCachedRegion() when the cached region has changed.
	void RefreshChildVisibleRegion(Rect cached_region) const override;

	// Only called by RegionUpdated() and CompositeChild().
	void RedrawChild(Rect region_to_update) const override;

	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) override;

private:
	// Never called, parent Table will do this by itself. 
	Ref<_WndBase_Impl*> HitTestChild(Point point) const override { Error(); return nullptr; }
};


END_NAMESPACE(WndDesign)