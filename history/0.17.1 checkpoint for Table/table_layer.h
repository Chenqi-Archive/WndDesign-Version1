#pragma once

#include "layer.h"


BEGIN_NAMESPACE(WndDesign)


class _Table_Impl;


// Table layer is the base layer of Table to accommodate all child windows.
// Table manages the child window structure so functions related with child windows will be 
//   redirected to parent Table to process. The layer only provides canvas for child windows,
//   and has no information about child windows.
// Because there's no overlapping of the child windows, the layer should be redirected.
// But it may also support scrolling later.
class TableLayer : public Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
public:
	TableLayer(_Table_Impl& parent);
	~TableLayer() {}

private:
	_Table_Impl& GetParent() const { return static_cast<_Table_Impl&>(_parent); }


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
	// These functions are called by parent Table to set canvas for child windows.
public:
	void SetChildCanvas(_WndBase_Impl& child, Rect region) {
		child.SetCanvas(Canvas(this, region));
	}


	// These functions are redirected to parent Table because only parent knows child window structure.
private:
	// Only called by RefreshCachedRegion() when the cached region has changed.
	void RefreshChildVisibleRegion(Rect cached_region) const override {
		GetParent().RefreshChildVisibleRegion(cached_region);
	}
	// Only called by RegionUpdated() and CompositeChild().
	void RedrawChild(Rect region_to_update) const override {
		GetParent().RedrawChild(region_to_update);
	}

	// These functions are NEVER called, parent Table will do this by itself. 
private:
	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) override { return; }
	Ref<_WndBase_Impl*> MouseHitTest(Point point) const override { return nullptr; }
};


END_NAMESPACE(WndDesign)