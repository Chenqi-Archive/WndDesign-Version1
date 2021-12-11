#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "tile.h"
#include "figure_queue.h"

#include <unordered_map>


BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;


using TileID = Point;
using TileIDLong = long long;
using TileRange = Rect;

class TargetResourceManager;

class TileCache {
	//////////////////////////////////////////////////////////
	////                 Resource Manager                 ////
	//////////////////////////////////////////////////////////
private:
	Ref TargetResourceManager* _resource_manager;

public:
	Ref TargetResourceManager* GetResourceManager() const { return _resource_manager; }
	Ref Target* UseTarget();
	void ReturnTarget(Ref Target* target);

	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	TileCache();
	~TileCache();

	void Init(Ref TargetResourceManager* resource_manager, Size tile_size, Color32 background,
			  Rect accessible_region, Rect visible_region, float cache_ratio);

	bool IsEmpty() const;
	void Clear();

private:
	Size _tile_size;
	Rect _accessible_region;

	float _cache_ratio;
	Rect _cached_region;  // The cached region always contains the visible region. 
	TileRange _cached_tile_range;

public:
	const Size GetTileSize() const { return _tile_size; }
	void SetAccessibleRegion(Rect accessible_region);
	const Rect GetAccessibleRegion() const { return _accessible_region; }

	const Rect GetCachedRegion() const { return _cached_region; }
	// When visible region falls out of cached region, reset the cached region.
	void ResetCachedRegion(Rect visible_region, Vector offset_hint = vector_zero);


	///////////////////////////////////////////////////////////
	////                    For Drawing                    ////
	///////////////////////////////////////////////////////////
private:
	Color32 _background;  // May use a figure instead.
	const Tile _tile_read_only;
	unordered_map<TileIDLong, Tile> _cache;
	uint _max_capacity;

	void SetCapacity(uint max_capacity);
	void SwapOut();
	Tile& WriteTile(TileID tile_id);
	const Tile& ReadTile(TileID tile_id) const;	

public:
	Color32 GetBackground() const { return _background; }
	// Clear the region with background color.
	void DrawBackground(Rect region);


public:
	enum class TileMaskAttribute :bool {
		AccessDenied = false,
		AccessOnly = true
	};
	void SetTileMask(Rect mask_region, TileMaskAttribute tile_mask_attribute);
	void ClearTileMask();
private:
	// Tile mask for redrawing child windows.
	TileRange _tile_mask;
	// If false, the masked tiles should not be accessed, 
    //   else if true, the masked region is the only permitted region.
	TileMaskAttribute _tile_mask_attribute;


public:
	// Draw the figures in the figure queue.
	// Return the updated region after all drawing commands.
	Rect DrawFigureQueue(FigureQueue& figure_queue, Vector position_offset, Rect bounding_region);

	// Draw the tile cache myself from the point to the region on another tile. For compositing.
	void DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const;
};

END_NAMESPACE(WndDesign)