#pragma once

#include "tile_cache.h"

#include "../style/wnd_style.h"
#include "../algorithm/space_index.h"

#include <list>


BEGIN_NAMESPACE(WndDesign)

using std::list;

class _WndBase_Impl;
class Canvas;


// The Abstract layer class.
class Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
protected:
	// The parent window.
	Ref _WndBase_Impl* _parent;

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

	Layer();
	~Layer();

	void Init(Ref _WndBase_Impl* parent, Rect composite_region, bool is_redirected,   // Relative to parent window.
			  Rect accessible_region, Point current_point,				// Relative to the layer itself.
			  Color32 background = ColorSet::White, WndStyle::RenderStyle style = WndStyle::RenderStyle());

	bool IsEmpty() const { return _parent == nullptr; }
	virtual void Clear();

public:
	const Rect& GetCompositeRegion() const { return _composite_region; }
	const Point GetCurrentPoint() const { return _current_point; }
	void SetStyle(WndStyle::RenderStyle style);
	bool IsOpaque() const { return _style.IsOpaque(); }
	bool IsRedirected() const { return _is_redirected; }

	// For already initialized as redirected layer to be unredirected.
	void SetUnredirected();


	// Helper functions.
	// Convert a point on the layer to the corresponding point on the parent window.
	const Point ConvertToParentPoint(Point point_on_layer) const {
		return point_on_layer + (_composite_region.point - _current_point);
	}
	// Convert a point on the parent window to the point on the layer.
	const Point ConvertToLayerPoint(Point point_on_parent) const {
		return point_on_parent - (_composite_region.point - _current_point);
	}


	//////////////////////////////////////////////////////////
	////                      Tiling                      ////
	//////////////////////////////////////////////////////////
protected:
	static constexpr uint max_tile_length = 1024;
	static constexpr uint tile_length_round_up = 32;
	static constexpr uint default_tile_length = 256;
	static inline uint RoundUpToMultipleOf(uint length, uint round) {
		uint remainder = length % round;
		if (remainder > 0) { return length + round - remainder; } else { return length; }
	}
	static inline Size CalculateTileSize(Size layer_size, bool width_change, bool height_change) {
		Size tile_size = layer_size;
		tile_size.width = RoundUpToMultipleOf(tile_size.width, tile_length_round_up);
		tile_size.height = RoundUpToMultipleOf(tile_size.height, tile_length_round_up);
		if (tile_size.width > max_tile_length || width_change) {
			tile_size.width = default_tile_length;
		}
		if (tile_size.height > max_tile_length || height_change) {
			tile_size.height = default_tile_length;
		}
		return tile_size;
	}

	// The tile cache that manages all the tiles, the accessible region and the visible region.
protected:
	Rect _visible_region;

public:
	// If the current region or the composite region has changed(when moving or scrolling),  
	//   recalulate the visible region, and maybe refresh the child windows.
	void RefreshVisibleRegion(Rect parent_visible_region = region_empty);

	// Called by child window's layers to refresh their visible regions.
	// For them, the visible region is the cached region.
	Rect GetVisibleRegion() const { 
		if (_is_redirected) {
			return _visible_region;
		} else {
			return _tile_cache.GetCachedRegion();
		}
	}

protected:
	TileCache _tile_cache;
	void SetAccessibleRegion(const Rect& accessible_region) { _tile_cache.SetAccessibleRegion(accessible_region); }
	const Rect& GetAccessibleRegion() const { return _tile_cache.GetAccessibleRegion(); }


	// For direct2d only. Only overrided by system layer.
private:
	virtual TargetResourceManager* AcquireResourceManager();


	///////////////////////////////////////////////////////////
	////                   Child Windows                   ////
	///////////////////////////////////////////////////////////
public:
	// When the layer's visible region has updated, refresh the child window's 
	//   visible region, and redraw the child windows.
	// Also used to force redraw child windows.
	virtual void RefreshChildVisibleRegion() const pure;

	// Test if any child window will receive the message.
	// Return the pointer to the child window if hit, or nullptr.
	virtual Ref _WndBase_Impl* MsgHitTest(Point point) const pure;


	///////////////////////////////////////////////////////////
	////                      Drawing                      ////
	///////////////////////////////////////////////////////////
protected:
	// Determine if the drawing commands by child window is passive.
	// If false, the parent window will recomposite immediately after the drawing, 
	bool _is_redrawing_child;

public:

	//virtual void RedrawChildWindow(Rect region_to_update) pure;

	// Draw a queue of figure on the layer, the figure will be painted on the region of the FigureContainer,
	//  plus the position_offset, inside the bounding_region of the layer.
	// Returns the updated region on this layer.
	Rect DrawFigureQueue(FigureQueue& figure_queue, Vector position_offset, Rect bounding_region,
						 bool commit_immediately = true);


	void DrawOn(Point point, Tile& target_tile, Rect region) const {
		_tile_cache.DrawOn(point, _style._opacity, target_tile, region);
	}
};



// A SingleWndLayer only contains a child window floating on the top.
class SingleWndLayer : public Layer {
public:
	SingleWndLayer();
	~SingleWndLayer();

	void Init(Ref _WndBase_Impl* parent, Rect composite_region, Ref _WndBase_Impl* wnd,
			  WndStyle::RenderStyle style);

protected:
	Ref _WndBase_Impl* _wnd;	// The child window.

public:
	void Clear() override;

	void RefreshChildVisibleRegion() const override;

	// Set the composite region when the layer is moving or resizing.
	// If size has changed, reset the accessible region and the child window's canvas.
	//   (For singlelayers, called by parent window)
	void SetCompositeRegion(const Rect& composite_region);

	Ref _WndBase_Impl* MsgHitTest(Point point) const override {
		return _wnd;
	}
};



class MultipleWndLayer : public Layer {
public:
	// For normal windows.
	void Init(Ref _WndBase_Impl* parent, Size size, bool is_redirected);

	// For scrollable windows.
	void Init(Ref _WndBase_Impl* parent, Rect composite_region, 
			  Rect accessible_region, Point current_point);

	~MultipleWndLayer();

	// Space index for sub windows.
protected:
	SpaceIndex<Ref _WndBase_Impl*> _wnds;

public:
	void AddWnd(Ref _WndBase_Impl* wnd, Rect region);
	void RemoveWnd(Ref _WndBase_Impl* wnd);
	void MoveWnd(Ref _WndBase_Impl* wnd, Rect new_region);
	void Clear() override { _wnds.Clear(); }

	void RefreshChildVisibleRegion() const override;

	Ref _WndBase_Impl* MsgHitTest(Point point) const override {
		return _wnds.QueryPoint(point);
	}

	// Set the current point when scrolling.
	//   (For multiplelayers, called by scrollable window)
	void SetCurrentPoint(Point current_point) {
		_current_point = current_point;
		RefreshVisibleRegion();  // Refresh the visible region for tile cache.
	}
};




END_NAMESPACE(WndDesign)