#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../algorithm/space_index.h"

#include "../texture/color.h"

#include "tile_cache.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::list;

class _WndBase_Impl;
class Canvas;

class Layer {
	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
protected:
	// The parent window.
	const Ref _WndBase_Impl* _parent;
	// The region on the parent window to composite.
	Rect _composite_region;   // Relative to parent window, {(0, 0), parent_size} for scrollable window layers.
	// The point on the layer at the starting composite point.
	Point _current_point;	  // Relative to the layer itself, (0, 0) for normal layers.
							  // The current region {_current_point, _composite_region.size} must be in the accessible region.
public:
	void Init(const Ref _WndBase_Impl* parent, Rect accessible_region, 
			  Rect composite_region, Point current_point = point_zero) {
		_parent = parent;
		_accessible_region = accessible_region;
		_composite_region = composite_region;
		_current_point = current_point;
		RefreshVisibleRegion();
	}

	// Set the current point when scrolling.
	void SetCurrentPoint(Point current_point) {
		_current_point = current_point;
		RefreshVisibleRegion();
	}
	Point GetCurrentPoint() const { return _current_point; }

	// Set the composite region when the layer is moving or resizing.
	void SetCompositeRegion(const Rect& composite_region) {
		_composite_region = composite_region;
		RefreshVisibleRegion();
	}
	const Rect& GetCompositeRegion() const { return _composite_region; }

	FigureContainer CreateFigureContainerForDrawing() const;

private:
	// The layer(often as a MultiWndLayer) may be redirected to the parent's own layer. (nullptr if not)
	// But it will also require the parent window to re-composite after rendering.
	const Ref Layer* _redirected_layer;
public:
	bool IsRedirected() const { return _redirected_layer != nullptr; }



	//////////////////////////////////////////////////////////
	////                      Tiling                      ////
	//////////////////////////////////////////////////////////
protected:
	TileCache _tile_cache;
public:
	Size CalculateTileSize(Size size_normal, Size size_min, Size size_max, bool may_resize) {
		return { 256px, 256px };
	}
	const Rect& GetVisibleRegion() const { return _tile_cache.GetVisibleRegion(); }
	// If the current region or the composite region has changed, refresh the visible region, 
	//    and may be reallocate the tiles, and refresh the child windows.
	virtual void RefreshVisibleRegion() {
		Rect parent_visible_region = _parent.GetVisibleRegion();
		_tile_cache.SetVisibleRegion(parent_visible_region.Intersect(_composite_region) - (_composite_region.point - _current_point));
	}


public:
	void SetAccessibleRegion(const Rect& accessible_region) {

	}
	const Rect& GetAccessibleRegion() const { return ; }


	///////////////////////////////////////////////////////////
	////                      Drawing                      ////
	///////////////////////////////////////////////////////////
public:
	void DrawFigureQueue(const queue<FigureContainer>& figure_queue, Rect region_offset)const{
		if (IsRedirected()) {
			Rect region_on_redirected_layer = _parent.GetCanvas().GetLayer()
			_redirected_layer->DrawFigureQueue(figure_queue, )
		}
		Rect updated_region = _tile_cache.DrawFigureQueue(figure_queue, region_offset);

	}
};


class SingleWndLayer : public Layer {
	//////////////////////////////////////////////////////////
	////                   Child window                   ////
	//////////////////////////////////////////////////////////
protected:
	const Ref _WndBase_Impl* _wnd;	// The child window.
public:
	void SetWnd(const Ref _WndBase_Impl* wnd);


	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
private:
	// compositing styles

	// Draw on the parent's canvas. Only contents in visible region will be drawn.
	void Render(Canvas& canvas) override;



	//////////////////////////////////////////////////////////
	////                      Tiling                      ////
	//////////////////////////////////////////////////////////
public:
	void RefreshVisibleRegion() override;
};



class MultipleWndLayer : public Layer {
	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	// Space index for sub windows.
	SpaceIndex<const Ref _WndBase_Impl*> _wnds;
public:
	void AddWnd(const Ref _WndBase_Impl* wnd);
	void RemoveWnd(const Ref _WndBase_Impl* wnd);


	///////////////////////////////////////////////////////////
	////                    Compositing                    ////
	///////////////////////////////////////////////////////////
private:
	// compositing styles

	// Draw on the parent's canvas. Only contents in visible region will be drawn.
	virtual void Render(Canvas& canvas);


	//////////////////////////////////////////////////////////
	////                      Tiling                      ////
	//////////////////////////////////////////////////////////
public:
	void RefreshVisibleRegion() override;
};



class SystemWndLayer : public SingleWndLayer {
	void RefreshVisibleRegion() override {
		// The visible region is always the whole current region.
		_visible_region = Rect{ _current_point, _composite_region.size };
		// Refresh the child window's visible region.
		_wnd.RefreshVisibleRegion();
	}
};


END_NAMESPACE(WndDesign)