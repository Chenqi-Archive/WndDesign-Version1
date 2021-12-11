#include "tile_cache.h"

#include "direct2d/direct2d.h"

// For maintaining updated tiles.
#include <set>
// For sqrt
#include <cmath>  

BEGIN_NAMESPACE(WndDesign)


// Divide a by b, and round the result down.
inline int div_floor(int a, int b) {
	if (a >= 0) { return a / b; } else { return (a - b + 1) / b; }
}
// Divide a by b, and round the result up.
inline int div_ceil(int a, int b) {
	if (a <= 0) { return a / b; } else { return (a + b - 1) / b; }
}

inline uint square(int x) {
	return x * x;
}
inline float Distance(const Point& a, const Point& b) {
	return static_cast<float>(sqrt(square(a.x - b.x) + square(a.y - b.y)));
}
inline float Distance(const Point& point, const Rect& rect) {
	int x_min = rect.point.x;
	int x_max = rect.point.x + static_cast<int>(rect.size.width) - 1;
	int y_min = rect.point.y;
	int y_max = rect.point.y + static_cast<int>(rect.size.height) - 1;
	int x = point.x;
	int y = point.y;
	if (x < x_min) {
		if (y < y_min) {
			return Distance(point, Point{ x_min, y_min });
		} else if (y <= y_max) {
			return static_cast<float>(x_min - x);
		} else {
			return Distance(point, Point{ x_min, y_max });
		}
	} else if (x <= x_max) {
		if (y < y_min) {
			return static_cast<float>(y_min - y);
		} else if (y <= y_max) {
			return 0.0;
		} else {
			return static_cast<float>(y - y_max);
		}
	} else {
		if (y < y_min) {
			return Distance(point, Point{ x_max, y_min });
		} else if (y <= y_max) {
			return static_cast<float>(x - x_max);
		} else {
			return Distance(point, Point{ x_max, y_max });
		}
	}
}

// The vector start from the origin point, and re
inline Vector ClipVectorInsideRectangle(int half_width, int half_height, Vector vector) {
	float shrink_ratio = 1;
	if (vector.x > half_width) { shrink_ratio = min(shrink_ratio, half_width / (float)vector.x); }
	if (vector.x < -half_width) { shrink_ratio = min(shrink_ratio, -half_width / (float)vector.x); }
	if (vector.y > half_height) { shrink_ratio = min(shrink_ratio, half_height / (float)vector.y); }
	if (vector.y < -half_height) { shrink_ratio = min(shrink_ratio, -half_height / (float)vector.y); }
	return { static_cast<int>(vector.x * shrink_ratio), static_cast<int>(vector.y * shrink_ratio) };
}
// Enlarge the region by length in ratio(> 1). And shift the larger region by offset_hint.
inline Rect EnlargeRegion(const Rect& region, float ratio, Vector offset_hint = vector_zero) {
	if (ratio <= 1.0) { return region; }
	Size size = region.size; 
	size.width = static_cast<uint>(static_cast<float>(size.width) * ratio);
	size.height = static_cast<uint>(static_cast<float>(size.height) * ratio);
	int width_padding = static_cast<int>(size.width - region.size.width) / 2;
	int height_padding = static_cast<int>(size.height - region.size.height) / 2;
	offset_hint = ClipVectorInsideRectangle(width_padding, height_padding, offset_hint);
	Point point = region.point;
	point.x = point.x - width_padding + offset_hint.x;
	point.y = point.y - height_padding + offset_hint.y;
	return { point, size };
}

inline TileRange RegionToOverlappedTileRange(Rect region, Size tile_size) {
	Point begin_point;
	begin_point.x = div_floor(region.point.x, tile_size.width);
	begin_point.y = div_floor(region.point.y, tile_size.height);
	Point end_point;
	end_point.x = div_ceil(region.RightBottom().x, tile_size.width);
	end_point.y = div_ceil(region.RightBottom().y, tile_size.height);
	Size size = { static_cast<uint>(end_point.x - begin_point.x), static_cast<uint>(end_point.y - begin_point.y) }; 
	return { begin_point, size };
}

inline TileRange RegionToInnerTileRange(Rect region, Size tile_size) {
	Point begin_point;
	begin_point.x = div_ceil(region.point.x, tile_size.width);
	begin_point.y = div_ceil(region.point.y, tile_size.height);
	Point end_point;
	end_point.x = div_floor(region.RightBottom().x, tile_size.width);
	end_point.y = div_floor(region.RightBottom().y, tile_size.height);
	Size size = {static_cast<uint>(end_point.x - begin_point.x), static_cast<uint>(end_point.y - begin_point.y) };
	return { begin_point, size };
}

inline Rect TileRangeToRegion(TileRange tile_range, Size tile_size) {
	Rect region;
	region.point.x = tile_range.point.x * static_cast<int>(tile_size.width);
	region.point.y = tile_range.point.y * static_cast<int>(tile_size.height);
	region.size.width = tile_range.size.width * tile_size.width;
	region.size.height = tile_range.size.height * tile_size.height;
	return region;
}

inline TileIDLong TileIDToLongLong(TileID tile_id) {
	union {
		TileID tile_id;
		TileIDLong tile_id_long;
	} convert;
	convert.tile_id = tile_id;
	return convert.tile_id_long;
}

inline TileID LongLongToTileID(TileIDLong tile_id_long) {
	union {
		TileID tile_id;
		TileIDLong tile_id_long;
	} convert;
	convert.tile_id_long = tile_id_long;
	return convert.tile_id;
}

// Iterate the points rowwise inside the rect.
class TileRangeIterator {
	TileID _start, _end;
	TileID _current_tile;

public:
	TileRangeIterator(TileRange rect) {
		_start = rect.LeftTop();
		_end = rect.RightBottom();
		_current_tile = _start;
	}
	void Next() {
		if (Finished()) { BreakPoint(); }
		_current_tile.x++;
		if (_current_tile.x >= _end.x) {
			_current_tile.x = _start.x;
			_current_tile.y++;
		}
	}
	void operator++() { Next(); }
	bool Finished() {
		return _current_tile.x >= _end.x || _current_tile.y >= _end.y;
	}
	TileID Item() const { return _current_tile; }
};



TileCache::TileCache() :
	_resource_manager(nullptr),
	_tile_size(size_max),
	_accessible_region(region_empty),
	_cache_ratio(1.0),
	_cached_region(region_empty),
	_cached_tile_range(region_empty),
	_background(ColorSet::White),
	_tile_read_only(this),
	_max_capacity(0),
	_tile_mask(region_empty),
	_tile_mask_attribute(TileMaskAttribute::AccessDenied) {
}

TileCache::~TileCache() {
	Clear();
}

void TileCache::Init(Ref TargetResourceManager* resource_manager, Size tile_size, Color32 background,
					 Rect accessible_region, Rect visible_region, float cache_ratio) {
	if (!IsEmpty()) { Clear(); }

	_resource_manager = resource_manager;

	_tile_size = tile_size;
	_accessible_region = accessible_region;

	_cache_ratio = cache_ratio;
	_cached_region = _accessible_region.Intersect(EnlargeRegion(visible_region, _cache_ratio));
	_cached_tile_range = RegionToOverlappedTileRange(_cached_region, _tile_size);
	SetCapacity(_cached_tile_range.Area());
	// Round to cached region to all the tiles related.
	_cached_region = _accessible_region.Intersect(TileRangeToRegion(_cached_tile_range, _tile_size));

	_background = background;
}

void TileCache::Clear() {
	_resource_manager = nullptr;

	_tile_size = size_max;
	_accessible_region = region_empty;
	_cache_ratio = 1.0;
	_cached_region = region_empty;
	_cached_tile_range = region_empty;
	_max_capacity = 0;

	_cache.clear();
}

bool TileCache::IsEmpty() const { 
	return _resource_manager == nullptr; 
}

Ref Target* TileCache::UseTarget() {
	return _resource_manager->UseTarget(_tile_size);
}

void TileCache::ReturnTarget(Ref Target* target) {
	_resource_manager->ReturnTarget(target);
}

void TileCache::SetAccessibleRegion(Rect accessible_region) { 
	_accessible_region = accessible_region; 
	// Reset the cached region.
	_cached_region = _accessible_region.Intersect(_cached_region);
	_cached_tile_range = RegionToOverlappedTileRange(_cached_region, _tile_size);
	SetCapacity(_cached_tile_range.Area());
	_cached_region = _accessible_region.Intersect(TileRangeToRegion(_cached_tile_range, _tile_size));
}

void TileCache::ResetCachedRegion(Rect visible_region, Vector offset_hint) {
	_cached_region = _accessible_region.Intersect(EnlargeRegion(visible_region, _cache_ratio, offset_hint));
	_cached_tile_range = RegionToOverlappedTileRange(_cached_region, _tile_size);
	SetCapacity(_cached_tile_range.Area());
	// Round to cached region to all the tiles related.
	_cached_region = _accessible_region.Intersect(TileRangeToRegion(_cached_tile_range, _tile_size));
}

void TileCache::SetCapacity(uint max_capacity) {
	_max_capacity = max_capacity;
	
	// Release all tiles if the region is empty.
	if (_max_capacity == 0) {
		_cache.clear();
	}
}

inline void TileCache::SwapOut() {
	auto it = _cache.begin();
	auto max_it = it;
	float max_distance = 0;
	for (; it != _cache.end(); ++it) {
		float distance = Distance(LongLongToTileID(it->first), _cached_tile_range);
		if (distance > max_distance) {
			max_distance = distance;
			max_it = it;
		}
	}
	if (max_it != _cache.end()) {
		_cache.erase(max_it);
	}
}

const Tile& TileCache::ReadTile(TileID tile_id) const {
	auto it = _cache.find(TileIDToLongLong(tile_id));
	if (it != _cache.end()) {
		return it->second;
	} else {
		return _tile_read_only;
	}
}

Tile& TileCache::WriteTile(TileID tile_id) {
	auto it = _cache.find(TileIDToLongLong(tile_id));
	if (it == _cache.end()) {
		// If not found, swap out the old tiles and insert a new one.
		do { SwapOut(); } while (_cache.size() >= _max_capacity);
		it = _cache.insert(std::make_pair(TileIDToLongLong(tile_id), Tile(this))).first;
	}
	return it->second;
}

void TileCache::SetTileMask(Rect mask_region, TileMaskAttribute tile_mask_attribute) {
	_tile_mask_attribute = tile_mask_attribute;
	if (_tile_mask_attribute == TileMaskAttribute::AccessDenied) {
		_tile_mask = RegionToInnerTileRange(mask_region, _tile_size);
	} else {
		_tile_mask = RegionToOverlappedTileRange(mask_region, _tile_size);
	}
}

void TileCache::ClearTileMask() {
	_tile_mask = region_empty;
	_tile_mask_attribute = TileMaskAttribute::AccessDenied;
}

void TileCache::DrawBackground(Rect region) {
	// Enumerate the related tiles for rendering.
	region = _cached_region.Intersect(region);
	TileRangeIterator it(RegionToOverlappedTileRange(region, _tile_size));
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();

		if ((_tile_mask.Contains(tile_id) ^ static_cast<bool>(_tile_mask_attribute)) == true) {
			continue;  // If the tile is masked, skip over.
		}

		Point tile_origin = { tile_id.x * static_cast<int>(_tile_size.width), tile_id.y * static_cast<int>(_tile_size.height) };
		Rect figure_region_on_tile = region - (tile_origin - point_zero);
		Tile& tile = WriteTile(tile_id);
		if (figure_region_on_tile.Contains({ point_zero, _tile_size })) {
			// The region fully overlaps the tile, just clear the tile.
			tile.Clear();
		} else {
			// Draw the background color on the region.
			tile.BeginDraw(figure_region_on_tile);
			tile.DrawColor(figure_region_on_tile, _background);
			tile.EndDraw();
		}
	}
}

Rect TileCache::DrawFigureQueue(FigureQueue& figure_queue, Vector position_offset, Rect bounding_region) {
	// Calculate the bounding region inside the cached region.
	bounding_region = _cached_region.Intersect(bounding_region);

	std::set<Ref Tile*> updated_tiles;
	Rect region_updated = region_empty;
	while (!figure_queue.IsEmpty()) {
		Alloc FigureContainer container = figure_queue.Pop();

		// The Figure's whole region on the layer.
		Rect figure_region = container.GetRegion() + position_offset;
		// The region to update on the layer.
		Rect region_to_draw = bounding_region.Intersect(figure_region);
		if (region_to_draw.IsEmpty()) {
			continue;
		}
		region_updated = region_updated.Union(region_to_draw);

		// Enumerate the related tiles for rendering.
		TileRangeIterator it(RegionToOverlappedTileRange(region_to_draw, _tile_size));
		for (; !it.Finished(); ++it) {
			TileID tile_id = it.Item();

			if ((_tile_mask.Contains(tile_id) ^ static_cast<bool>(_tile_mask_attribute)) == true) { 
				continue;  // If the tile is masked, skip over.
			}

			Point tile_origin = { tile_id.x * static_cast<int>(_tile_size.width), tile_id.y * static_cast<int>(_tile_size.height) };

			Rect figure_region_on_tile = figure_region - (tile_origin - point_zero);
			Rect bounding_region_on_tile = bounding_region - (tile_origin - point_zero);

			Tile& tile = WriteTile(tile_id);
			tile.BeginDraw(bounding_region_on_tile); // Reentrantable.
			container.GetFigure().DrawOn(tile, figure_region_on_tile);

			// Save the pointer of the tile.
			// The pointer will never be invalid, because the active tiles will not be swapped out.
			updated_tiles.insert(Ref &tile);
		}

		// The figure has finished painting on all related tiles.
		container.Destroy();
	}
	
	// Commit all related tiles.
	for (Ref Tile* it : updated_tiles) {
		it->EndDraw();
	}

	// Nothing has been drawed. Maybe just for flushing, return the bounding_region.
	if (region_updated == region_empty) {
		return bounding_region;
	}

    return region_updated;
}


void TileCache::DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const {
	Rect my_region = { point, region.size };
	TileRangeIterator it(RegionToOverlappedTileRange(my_region, _tile_size));
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Point tile_origin = { tile_id.x * static_cast<int>(_tile_size.width), tile_id.y * static_cast<int>(_tile_size.height) };
		Rect rect_on_tile = (my_region - (tile_origin - point_zero)).Intersect({ point_zero, _tile_size });
		// The tile has begun drawing, draw directly.
		const Tile& tile = ReadTile(tile_id);
		tile.DrawOn(rect_on_tile.point,
					opacity,
					target_tile,
					rect_on_tile + (tile_origin - my_region.point) + (region.point - point_zero));
	}
}


END_NAMESPACE(WndDesign)