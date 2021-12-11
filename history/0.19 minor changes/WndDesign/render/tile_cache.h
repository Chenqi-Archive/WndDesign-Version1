#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "tile.h"
#include "figure_queue.h"

#include <unordered_map>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;  // For caching tiles.
using std::vector;  // For maintaining active tiles.

class TargetResourceManager;
class Layer;

using TileID = Point;
using TileIDLong = long long;
using TileRange = Rect;


#pragma warning (push)
#pragma warning (disable : 26495) // Always initialize a member variable.
#pragma warning (disable : 26812) // Prefer 'enum class' over 'enum'.
struct CachePolicy {  // 6 bytes.
public:
	enum Policy : uchar {
		CacheAll,              // The entire accessible_region is cached.
		EnlargeVisibleRegion,  // The visible enlarged by length pixels is cached. 
		ScaleVisibleRegion,    // The region scaled by the visible region is cached.
	} _policy;
	uchar _extra_capacity;	   // The extra capacity for cached tiles.
	union {
		uint _enlarge_length;  // EnlargeVisibleRegion
		float _cache_ratio;    // ScaleVisibleRegion
	};
public:
	explicit CachePolicy(Policy policy, uchar extra_capacity = 0, float cache_ratio = 1, uint enlarge_length = 0)
		: _policy(policy), _extra_capacity(extra_capacity) {
		switch (_policy) {
		case CacheAll: break;
		case EnlargeVisibleRegion: _enlarge_length = enlarge_length; break;
		case ScaleVisibleRegion:_cache_ratio = cache_ratio; break;
		default: _policy = ScaleVisibleRegion; _cache_ratio = cache_ratio; break;
		}
	}
};
#pragma warning (pop)


class TileCache : Uncopyable{
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	TileCache(Ref<TargetResourceManager*> resource_manager, const Background& background,
			  Rect accessible_region, Rect visible_region, CachePolicy cache_policy);
	~TileCache();

private:
	Size _tile_size;
	Rect _accessible_region;
	const Background& _background;
public:
	const Rect GetAccessibleRegion() const { return _accessible_region; }
	const Background& GetBackground() const { return _background; }
	// Set the accessible region, recalculate the tile size, and reset the cached region. 
	// Return true if tile size has changed and the layer will redraw.
	void SetAccessibleRegion(Rect accessible_region, Rect visible_region);


	///////////////////////////////////////////////////////////
	////                      Caching                      ////
	///////////////////////////////////////////////////////////
private:
	CachePolicy _cache_policy;
	Rect _cached_region;
	TileRange _cached_tile_range;
	uint _max_capacity;
	unordered_map<TileIDLong, Tile> _cache;
	const Tile _tile_read_only;

private:
	void SwapOut();
	Tile& WriteTile(TileID tile_id);
	const Tile& ReadTile(TileID tile_id) const;

public:
	const Rect GetCachedRegion() const { return _cached_region; }
	// When visible region falls out of cached region, reset the cached region.
	// The layer must make sure that when the cached region has changed, 
	//   all related child windows should be redrawn.
	void SetCachedRegion(Rect visible_region, Vector offset_hint = vector_zero);


	//////////////////////////////////////////////////////////
	////                 Resource Manager                 ////
	//////////////////////////////////////////////////////////
private:
	Ref<TargetResourceManager*> _resource_manager;
public:
	Ref<TargetResourceManager*> GetResourceManager() const { return _resource_manager; }
	void SetResourceManager(Ref<TargetResourceManager*> resource_manager);
	Alloc<Target*> UseTarget() const;
	void ReturnTarget(Alloc<Target*> target) const;


	///////////////////////////////////////////////////////////
	////                    For Drawing                    ////
	///////////////////////////////////////////////////////////
	// Tile mask for redrawing child windows.
private:
	TileRange _tile_mask;
public:
	void SetTileMask(Rect mask_region);
	void ClearTileMask();

private:
	// The tiles that are being redrawed.
	// The pointer will never be invalid, because the active tiles will not be swapped out.
	// Reentrance is permitted.
	mutable vector<Ref<Tile*>> _active_tiles;

public:
	// Clear the region with background.
	void DrawBackground(Rect region);
	// Draw the figures in the figure queue.
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset);
	// Commit all active tiles.
	void CommitActiveTiles() const;
	// Draw the tile cache myself from the point to the region on another tile. For compositing.
	void DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const;
};


END_NAMESPACE(WndDesign)