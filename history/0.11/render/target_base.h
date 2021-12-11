#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "render_target.h"

BEGIN_NAMESPACE(WndDesign)

class RenderLayer;

class RenderTargetBase {
private:
	// The parent render layer.
	Ref RenderLayer* _layer;

protected:
	// The total size of the target.
	// Extra large target will use tiles to reduce memory usage.
	Size size;

public:
	// Create a new RenderTarget inside the current render target(the intersection), 
	//   the overflow region will be disgarded.
	RenderTarget CreateRenderTarget(const Rect& sub_region) const;


	virtual bool BeginDraw(const Rect& rect) const pure;
	virtual void EndDraw() const pure;
};


class HWNDRenderTarget : public RenderTargetBase {
	using HWND = void*;



};

class BitmapRenderTarget : public RenderTargetBase {

	// Write on allocate.

};


constexpr Size tile_size = { 256px, 256px };

class TileRenderTarget : public RenderTargetBase {
	using TileID = Point;
	using Tile = BitmapRenderTarget;

private:
	Point _current_point;
	// TileID _left_top_tile = _current_point % tile_size;


	class TileCache {
	private:
		uint _size = 16;  // Should automatically change according to the window size 
						  //   or the using pattern.

	public:
		void SetSize(uint size);

		Ref Tile* GetTile(TileID tile);
	};


};




END_NAMESPACE(WndDesign)