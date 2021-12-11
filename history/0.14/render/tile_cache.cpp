#include "tile_cache.h"

#include "direct2d/target.h"

// For sqrt
#include <cmath>  

BEGIN_NAMESPACE(WndDesign)

// The max size of a tile when region size does not change, if changed, use the variable sizes below.
constexpr Size tile_size_max_fixed = { 1024, 1024 };
// Used as the initial previous_tile_size.
constexpr Size tile_size_empty = { 32, 32 };

// The variable tile sizes.
constexpr uint length_round_up = 32;
constexpr Size tile_size_min = { 64, 64 };
constexpr Size tile_size_max = { 384, 384 };


inline uint RoundUpToMultipleOf(uint length, uint round) {
	uint remainder = length % round;
	if (remainder > 0) { return length + round - remainder; } else { return length; }
}

inline Size CalculateTileSize(Size entire_size, Size previous_tile_size = tile_size_empty) {
	Size tile_size;
	// For empty region, return the initial size.
	if (entire_size.IsEmpty()) { return previous_tile_size; }
	// For the first time to calculate, assume the size will be fixed.
	if (previous_tile_size == tile_size_empty) {
		// Round up the length.
		tile_size.width = RoundUpToMultipleOf(entire_size.width, length_round_up);
		tile_size.height = RoundUpToMultipleOf(entire_size.height, length_round_up);
		// If the length is larger than the max fixed tile length, use variable tile size.
		if (tile_size.width > tile_size_max_fixed.width) { tile_size.width = tile_size_max.width; }
		if (tile_size.height > tile_size_max_fixed.height) { tile_size.height = tile_size_max.height; }
		// The length should be greater than the min tile length.
		if (tile_size.width < tile_size_min.width) { tile_size.width = tile_size_min.width; }
		if (tile_size.height < tile_size_min.height) { tile_size.height = tile_size_min.height; }
		return tile_size;
	}
	// The tile size need to be recalculated, the calculation below for width and height is independent.
	// For width:
	if (previous_tile_size.width == RoundUpToMultipleOf(entire_size.width, length_round_up)) {
		// If the entire region has not changed, use the previous tile size:
		tile_size.width = previous_tile_size.width;
	} else {
		// The entire region has changed, use variable tile sizes:
		if (previous_tile_size.width >= tile_size_max.width) {
			// If previous tile size is larger than the max variable tile size, use the max tile size.
			tile_size.width = tile_size_max.width;
		} else {
			if (entire_size.width <= previous_tile_size.width * 3) {
				// If previous tile size multiplied by 3 is bigger than the region size, use the previous size.
				tile_size.width = previous_tile_size.width;
			} else {
				// Else, enlarge the tile size. The new length must be greater than pervious tile length.
				tile_size.width = RoundUpToMultipleOf(entire_size.width, length_round_up * 2) / 2;
				// If the length is larger than the max tile length, use max tile size.
				if (tile_size.width > tile_size_max.width) { tile_size.width = tile_size_max.width; }
			}
		}
	}
	// For height: use the same rule.
	if (previous_tile_size.height == RoundUpToMultipleOf(entire_size.height, length_round_up)) {
		tile_size.height = previous_tile_size.height;
	} else {
		if (previous_tile_size.height >= tile_size_max.height) {
			tile_size.height = tile_size_max.height;
		} else {
			if (entire_size.height <= previous_tile_size.height * 3) {
				tile_size.height = previous_tile_size.height;
			} else {
				tile_size.height = RoundUpToMultipleOf(entire_size.height, length_round_up * 2) / 2;
				if (tile_size.height > tile_size_max.height) { tile_size.height = tile_size_max.height; }
			}
		}
	}
	return tile_size;
}


// Divide a by b, and round the result down.
inline int div_floor(int a, int b) {
	if (a >= 0) { return a / b; } else { return (a - b + 1) / b; }
}
// Divide a by b, and round the result up.
inline int div_ceil(int a, int b) {
	if (a <= 0) { return a / b; } else { return (a + b - 1) / b; }
}

inline int square(int x) {
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

// Scale the region by length in ratio(> 1). And shift the larger region by offset_hint.
inline Rect ScaleRegion(const Rect& region, float ratio, Vector offset_hint = vector_zero) {
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

inline Rect EnlargeRegion(Rect region, uint enlarge_length, Vector offset_hint = vector_zero) {
	offset_hint = ClipVectorInsideRectangle(enlarge_length, enlarge_length, offset_hint);
	region.point.x = region.point.x - static_cast<int>(enlarge_length) + offset_hint.x;
	region.point.y = region.point.y - static_cast<int>(enlarge_length) + offset_hint.x;
	region.size.width += 2 * enlarge_length;
	region.size.height += 2 * enlarge_length;
	return region;
}

inline Rect CalculateCachedRegion(Rect visible_region, CachePolicy policy, Vector offset_hint = vector_zero) {
	switch (policy._policy) {
	case CachePolicy::CacheAll: 
		return region_infinite;
	case CachePolicy::EnlargeVisibleRegion: 
		return EnlargeRegion(visible_region, policy._enlarge_length, offset_hint);
	case CachePolicy::ScaleVisibleRegion:
		return ScaleRegion(visible_region, policy._cache_ratio, offset_hint);
	}
	return region_empty;
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

inline Point GetTileOrigin(TileID tile_id, Size tile_size) {
	return { tile_id.x * static_cast<int>(tile_size.width), tile_id.y * static_cast<int>(tile_size.height) };
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
		if (Finished()) { Error(); }
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



TileCache::TileCache(Ref TargetResourceManager* resource_manager, Color32 background,
					 Rect accessible_region, Rect visible_region, CachePolicy cache_policy) :
	_tile_size(CalculateTileSize(accessible_region.size)),
	_accessible_region(accessible_region),
	_background(background),
	_cache_policy(cache_policy),
	_tile_read_only(this),
	_resource_manager(resource_manager),
	_tile_mask(region_empty) {
	SetCachedRegion(visible_region);
}

void TileCache::SetAccessibleRegion(Rect accessible_region, Rect visible_region) {
	_accessible_region = accessible_region; 
	Size new_tile_size = CalculateTileSize(_accessible_region.size, _tile_size);
	if (new_tile_size != _tile_size) { // If tile size changed, reset the tile size, and clear the tile_cache.
		_tile_size = new_tile_size;
		_cache.clear();
	}
	SetCachedRegion(visible_region);  // Reset the cached region.
}

void TileCache::SetCachedRegion(Rect visible_region, Vector offset_hint) {
	// The cached region cannot be reset when drawing.
	if (!_active_tiles.empty()) { Error(); }
	Rect cached_region = CalculateCachedRegion(visible_region, _cache_policy, offset_hint);

	cached_region = _accessible_region.Intersect(cached_region);
	_cached_tile_range = RegionToOverlappedTileRange(cached_region, _tile_size);
	_cached_region = _accessible_region.Intersect(TileRangeToRegion(_cached_tile_range, _tile_size));

	_max_capacity = _cached_tile_range.Area() + _cache_policy._extra_capacity;
	if (_max_capacity == 0) { _cache.clear(); } // Release all tiles if the region is empty.
	_cache.reserve(_max_capacity);
	_active_tiles.reserve(_max_capacity);
}

void TileCache::SwapOut() {
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
		if (max_it->second._has_begun) { Error(); }
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
		it = _cache.emplace(TileIDToLongLong(tile_id), this).first;
	#pragma message("tile")
	}
	return it->second;
}

void TileCache::SetResourceManager(Ref TargetResourceManager* resource_manager) {
	if (_resource_manager == resource_manager) { return; }
	if (_resource_manager != nullptr) {
		_cache.clear(); // Clear old resources.
	}
	_resource_manager = resource_manager;
}

Ref Target* TileCache::UseTarget() const {
	return _resource_manager->UseTarget(_tile_size);
}

void TileCache::ReturnTarget(Ref Target* target) const {
	_resource_manager->ReturnTarget(target);
}

void TileCache::SetTileMask(Rect mask_region) {
	_tile_mask = RegionToInnerTileRange(mask_region, _tile_size);
}

void TileCache::ClearTileMask() {
	_tile_mask = region_empty;
}

void TileCache::DrawBackground(Rect region) {
	// Enumerate the related tiles for rendering.
	region = _cached_region.Intersect(region);
	for (TileRangeIterator it(RegionToOverlappedTileRange(region, _tile_size)); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		if (_tile_mask.Contains(tile_id)) {
			continue;  // If the tile is masked, skip over.
		}
		Point tile_origin = GetTileOrigin(tile_id, _tile_size);
		Rect region_to_draw = region - (tile_origin - point_zero);
		Tile& tile = WriteTile(tile_id);
		if (region_to_draw.Contains({ point_zero, _tile_size })) {
			tile.Clear(); // The region overlaps the entire tile, just clear the tile.
		} else {
			// Begin drawing the tile, and keep it in a vector.
			if (tile.BeginDraw(region_to_draw)) {
				_active_tiles.push_back(Ref & tile);
			}
			tile.DrawColor(region_to_draw, _background);  // Draw the background color on the region.
		}
	}
}

void TileCache::DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) {
	// Calculate the bounding region inside the cached region.
	bounding_region = _cached_region.Intersect(bounding_region);
	// Find all tiles overlapping with the bounding region and begin draw.
	for (TileRangeIterator it(RegionToOverlappedTileRange(bounding_region, _tile_size)); !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		if (_tile_mask.Contains(tile_id)) { continue; }// If the tile is masked, skip over.

		Point tile_origin = GetTileOrigin(tile_id, _tile_size);
		Rect bounding_region_on_tile = bounding_region - (tile_origin - point_zero);
		Tile& tile = WriteTile(tile_id);
		// If the tile has alrealy begun, the bounding region will be updated, and it will not be inserted again.
		if (tile.BeginDraw(bounding_region_on_tile)) {
			_active_tiles.push_back(Ref & tile);
		}
	}

	// Draw each figure on its related tiles.
	for (uint pos = 0; pos < figure_queue.Size(); ++pos) {
		const FigureContainer& container = figure_queue.Get(pos);

		// The Figure's entire region on the layer.
		Rect figure_region = container.GetRegion() + position_offset;
		// The region to update on the layer.
		Rect region_to_draw = bounding_region.Intersect(figure_region);
		if (region_to_draw.IsEmpty()) { continue; }

		// Enumerate the related tiles for drawing.
		for (TileRangeIterator it(RegionToOverlappedTileRange(region_to_draw, _tile_size)); !it.Finished(); ++it) {
			TileID tile_id = it.Item();
			if (_tile_mask.Contains(tile_id)) { continue; } // If the tile is masked, skip over.

			Point tile_origin = GetTileOrigin(tile_id, _tile_size);
			Rect figure_region_on_tile = figure_region - (tile_origin - point_zero);
			Tile& tile = WriteTile(tile_id);
			if (!tile._has_begun) { Error(); } // The tile must have begun drawing.
			container.GetFigure().DrawOn(tile, figure_region_on_tile);
		}
	}
}

void TileCache::CommitActiveTiles() const {
	for (Ref Tile* tile : _active_tiles) {
		tile->EndDraw();
	}
	_active_tiles.clear();
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