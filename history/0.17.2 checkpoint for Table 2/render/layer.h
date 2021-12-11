#pragma once

#include "tile_cache.h"

#include "../style/wnd_style.h"
#include "../style/scroll_wnd_style.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;  // For MultipleWndLayer to keep child windows.

class _WndBase_Impl;


class Layer : Uncopyable{
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
protected:
	// The parent window.
	_WndBase_Impl& _parent;

	// The region on the parent window to composite.
	Rect _composite_region;   // Relative to parent window, {point_zero, parent_size} for scrollable window layers.

	// The point on the layer at the starting composite point.
	Point _current_point;	  // Relative to the layer itself, (0, 0) for normal layers.
							  // The current region {_current_point, _composite_region.size} must be in the accessible region.

	// The layer(often as a MultiWndLayer at the most bottom) may be redirected to parent's own layer,
	//   but it will also require the parent window to re-composite after rendering.
	// A redirected layer may have child windows, and can be drawed on.
	// System layer created by desktop is never redirected.
	bool _is_redirected;

	// The layer's composition style.
	WndStyle::RenderStyle _style;

public:
	Layer(_WndBase_Impl& parent, Rect composite_region, bool is_redirected,   // Relative to parent window.
		  Rect accessible_region, Point current_point,				// Relative to the layer itself.
		  const Background& background, WndStyle::RenderStyle style = WndStyle::RenderStyle());

	// Functions below are only called by parent windows. Layer itself should never call these functions.
public:
	Ref<_WndBase_Impl*> GetParent() const { return &_parent; }
	const Rect GetCompositeRegion() const { return _composite_region; }
	const Point GetCurrentPoint() const { return _current_point; }
	const Rect GetAccessibleRegion() const { return _tile_cache.GetAccessibleRegion(); }
	bool IsRedirected() const { return _is_redirected; }

	// Helper functions.
	// Convert a point on the parent window to the point on the layer.
	const Point ConvertToLayerPoint(Point point_on_parent) const {
		return point_on_parent - (_composite_region.point - _current_point);
	}
	// Convert a point on the layer to the corresponding point on the parent window.
	const Point ConvertToParentPoint(Point point_on_layer) const {
		return point_on_layer + (_composite_region.point - _current_point);
	}
	// Convert a point on the layer to the corresponding point on the desktop.
	const Point ConvertToDesktopPoint(Point point_on_layer) const;

	const Rect ConvertToLayerRegion(Rect region_on_parent) const {
		return Rect(ConvertToLayerPoint(region_on_parent.point), region_on_parent.size);
	}
	const Rect ConvertToParentRegion(Rect region_on_layer) const {
		return Rect(ConvertToParentPoint(region_on_layer.point), region_on_layer.size);
	}


	//////////////////////////////////////////////////////////
	////                      Tiling                      ////
	//////////////////////////////////////////////////////////
protected:
	TileCache _tile_cache;
public:
	// The visible region for a window is the cached region on a layer.
	// The cached region for the layer is determined by the layer's parent window's visible region.
	Rect GetCachedRegion() const;

	// If the current region or the composite region has changed(when moving or scrolling),  
	//   recalulate the cached region, and maybe refresh the child windows.
	// visible_region is relative to the layer.
	void RefreshCachedRegion(Rect visible_region);

protected:
	// Get the visible region on layer, for refreshing cached region.
	// Only overrided by system layer.
	virtual Rect GetLayerVisibleRegion() const ;


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
private:
	// Indicating if the layer is drawing child windows to avoid reentrance.
	mutable bool _is_redrawing_child;

	// There are some virtual functions below. They are actually not vitual function calls,
	//   but rather a uniform interface, the type of the layer has been determined by its parent window.

	// When the layer's cached region has updated, refresh the child window's 
	//   visible region, and redraw the child windows.
	// Only called by refresh cached region.
	virtual void RefreshChildVisibleRegion(Rect cached_region) const pure;

	// Draw child windows. 
	// Only called by RegionUpdated.
	virtual void RedrawChild(Rect region_to_update) const pure;

public:
	// Called by canvas when a child window is updated, need to recomposite.
	// Or by the layer when the cached region has changed.
	// Or by parent window when adding or removing a child window.
	void RegionUpdated(Rect region_to_update, bool commit_immediately) const;

	// For redirected layer, called by parent window when compositing.
	void CompositeChild(Rect region_to_update) const;

	// Do hit test for mouse messages. 
	// Returns the pointer to the child window if the message is to be handled by a child window, 
	//   else return nullptr and parent window will handle the message.
	// The point should be relative to the layer, not parent window.(For ScrollWnd)
	virtual Ref<_WndBase_Impl*> MouseHitTest(Point point) const pure;

public:
	// The layer must be drawing child.
	// Draw a queue of figure on the layer, the figure will be painted on the region of the FigureContainer,
	//  plus the position_offset, inside the bounding_region of the layer.
	// Only overrided by system layer to draw directly on system target.
	virtual void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset);

	// Draw myself on a tile, for compositing.
	void DrawOn(Point point, Tile& target_tile, Rect region) const {
		if (_style._opacity == 0) { return; }
		_tile_cache.DrawOn(point, _style._opacity, target_tile, region);
	}


	// Used by tile_cache for direct2d rendering.
public:
	void SetResourceManager(Ref<TargetResourceManager*> resource_manager) {
		_tile_cache.SetResourceManager(resource_manager);
		SetChildResourceManager(resource_manager);
	}
	Ref<TargetResourceManager*> GetResourceManager() const {return _tile_cache.GetResourceManager();}
private:
	// Acquire resource manager from parent.
	Ref<TargetResourceManager*> AcquireResourceManager();
	// Set resource manager from child windows.
	virtual void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) pure;
};



// A SingleWndLayer only contains a child window floating on the top.
class SingleWndLayer : public Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
public:
	SingleWndLayer(_WndBase_Impl& parent, Rect composite_region,
				   Ref<_WndBase_Impl*> wnd, WndStyle::RenderStyle style,
				   Rect relative_region = region_infinite, const Padding& padding = padding_null);
	~SingleWndLayer();

	// Functions below are only called by parent windows. Layer itself should never call these functions.
public:
	bool IsOpaque() const { return _style.IsOpaque(); }
	uchar GetZIndex() const { return _style._z_index; }
	void SetStyle(WndStyle::RenderStyle style) { _style = style; }
	// Set the composite region when the layer is moving or resizing.
	// If size has changed, reset the accessible region and the child window's canvas.
	//   (For singlelayers, called by parent window)
	void SetCompositeRegion(const Rect& composite_region);


	// For sticky layers, used by scroll window only.
private:
	// Relative region and padding are saved for convenience when scrolling.
	Rect _relative_region;  // The region relative to the base layer. (region_infinite for normal layers)
	Padding _padding;
public:
	bool IsSticky() const { return _relative_region != region_infinite; }
	const Rect& GetRelativeRegion() const { return _relative_region; }
	const Padding& GetPadding() const { return _padding; }
	void SetRelativeRegion(Rect relative_region) { _relative_region = relative_region; }
	void SetPadding(Padding padding) { _padding = padding; }


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
private:
	Ref<_WndBase_Impl*> _wnd;	// The child window.

public:
	Ref<_WndBase_Impl*> GetWnd() const { return _wnd; }

private:
	// Only called by RefreshCachedRegion.
	void RefreshChildVisibleRegion(Rect cached_region) const override;
	// Only called by RegionUpdated.
	void RedrawChild(Rect region_to_update) const override;

	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) override;

public:
	Ref<_WndBase_Impl*> MouseHitTest(Point point) const override;
};



class MultipleWndLayer : public Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
public:
	// For normal windows. The parent size is empty at construction, so the size of accessible region 
	//   and composite region will be empty. The layer size will be calculated when parent sets canvas.
	MultipleWndLayer(_WndBase_Impl& parent);
	
	// For scrollable windows.
	MultipleWndLayer(_WndBase_Impl& parent, ScrollWndStyle::RegionStyle entire_region);

	~MultipleWndLayer();

private:
	ScrollWndStyle::RegionStyle _entire_region;


	// Functions below are only called by parent windows. Layer itself should never call these functions.
public:
	void SetUnredirected();

	// The parent window's size has changed, reset the size according to entire_region style, 
	//   and current point will also be recalculated. 
	// Return true if accessible region has changed. (Useful for scrollwnd)
	bool SetCompositeSize(Size parent_size);

	// Set the current point when scrolling. (For multiplelayers, called by scrollable window)
	// Return true if current point has changed.
	bool SetCurrentPoint(Point point);


	///////////////////////////////////////////////////////////
	////             Child Windows and Drawing             ////
	///////////////////////////////////////////////////////////
	// Space index for child windows.
private:
	struct WndObject { 
		Ref<_WndBase_Impl*> wnd; 
		Rect region; 
	};
	list<WndObject> _wnds;

private:
	// Only called by RefreshCachedRegion.
	void RefreshChildVisibleRegion(Rect cached_region) const override;
	// Only called by RegionUpdated, and CompositeChild for redirected layer.
	void RedrawChild(Rect region_to_update) const override;

	void SetChildResourceManager(Ref<TargetResourceManager*> resource_manager) override;

public:
	Ref<_WndBase_Impl*> MouseHitTest(Point point) const override;
	void AddWnd(Ref<_WndBase_Impl*> wnd, Rect region);
	void RemoveWnd(Ref<_WndBase_Impl*> wnd);
	void MoveWnd(Ref<_WndBase_Impl*> wnd, Rect new_region);
	void ClearChild();  // For normal wnd when canvas is changed.
	void ResetChildRegion(); // For scrollwnd when accessible region is changed.
};



END_NAMESPACE(WndDesign)