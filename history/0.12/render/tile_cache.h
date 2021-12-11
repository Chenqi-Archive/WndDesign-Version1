#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "render_target.h"

#include <unordered_map>


BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;


using TileID = Point;
using TileIDLong = long long;
using TileRange = Rect;


class FigureContainer {
private:
	const Alloc Ref Figure* figure;
	Rect initial_region;
	bool should_destroy;
public:
	FigureContainer(const Alloc Ref Figure* figure, Rect region, bool should_destroy = true) 
		: figure(figure), initial_region(region), should_destroy(should_destroy){}
	void DestroyFigure() { if (should_destroy) { delete figure; } }
};


class TileCache {
private:
	// The base target to create compatable bitmaps.
	Ref TargetImpl* _compatable_target;

	TargetImplPool _target_base_pool;
public:
	// Only for child tiles to get the render target.
	Alloc TargetImpl* BorrowTargetBase();
	void ReturnTargetBase(Ref TargetImpl* target);

private:
	Rect _accessible_region;
	Rect _visible_region;
	Size _tile_size;  // _tile_size can not be changed after the layer is created.

public:
	uint _background_color;

private:
	TileRange GetOverlappedTiles(Rect rect);

private:
	using Tile = RenderTarget;
	const Tile _tile_read_only;

	// A tile could not be copied after it was in cache, or the 
	//   render target it contains may be returned twice.
	std::unordered_map<TileIDLong, Tile> _cache;

	uint _max_capacity;

	void SwapOut();

	const Tile& ReadTile(TileID tile_id);

	Tile& WriteTile(TileID tile_id);

public:
	TileCache();
	~TileCache();
	void Init(Ref TargetImpl* compatable_target, Rect accessible_region, Rect visible_region, Size tile_size);
	void Clear();
	bool IsEmpty() const;
	void SetVisibleRegion(Rect visible_region);
	Rect GetVisibleRegion() const { return _visible_region; }

public:
	// Return the updated region after all drawing commands.
	Rect DrawFigureQueue(const queue<FigureContainer>& figure_queue, Rect region_offset) const;
};

END_NAMESPACE(WndDesign)