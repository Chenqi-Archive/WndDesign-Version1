#pragma once

#include <d2d1.h>
#include <dwrite.h>

#undef max
#undef min
#include "../../WndDesign/WndDesign/geometry/geometry.h"

#include <unordered_map>
#include <stack>
#include <cmath>

//#define Debug

#ifdef Debug
#include <cstdio>
#define debug_printf(format_string, ...) printf(format_string, __VA_ARGS__)
#else
#define debug_printf(format_string, ...)
#endif


using namespace WndDesign;


template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


inline int div_floor(int a, int b) {
	if (a >= 0) { return a / b; }
	else { return (a - b + 1) / b; }
}
inline int mod_floor(int a, int b) {
	int res = a % b;
	if (res < 0) { return res + b; }
	return res;
}

/*
	|		|
-----------------
	|		|
-----------------
	|		|
*/

inline uint square(int x) {
	return x * x;
}
inline float Distance(const Point& a, const Point& b) {
	return sqrt(square(a.x - b.x) + square(a.y - b.y));
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
			return x_min - x;
		} else {
			return Distance(point, Point{ x_min, y_max });
		}
	} else if (x <= x_max) {
		if (y < y_min) {
			return y_min - y;
		} else if (y <= y_max) {
			return 0;
		} else {
			return y - y_max;
		}
	} else {
		if (y < y_min) {
			return Distance(point, Point{ x_max, y_min });
		} else if (y <= y_max) {
			return x - x_max;
		} else {
			return Distance(point, Point{ x_max, y_max });
		}
	}
}


using TileID = Point;
using TileIDLong = long long;
using TileRange = Rect;

inline TileIDLong ConvertTileIDToLong(TileID tile_id) {
	union {
		TileID tile_id;
		TileIDLong tile_id_long;
	} convert;
	convert.tile_id = tile_id;
	return convert.tile_id_long;
}
inline TileID ConvertLongToTileID(TileIDLong tile_id_long) {
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
	void Next() throw(std::exception) {
		if (Finished()) { throw std::exception("Invalid Iterator!"); }
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


class TileCache;

class Tile {
private:
	TileCache* _parent;
	Alloc ID2D1BitmapRenderTarget* _target;  // Borrowed target.
	bool _is_allocated;
public:
	Tile(TileCache* parent);
	Tile(const Tile& tile);
	void operator=(const Tile& tile) = delete;

	~Tile();
	void DrawColor(uint color, Rect rect);

public:
	void DrawOn(Rect region_on_tile, ID2D1RenderTarget* target, Point position) const;
};


class TargetPool {
private:
	std::stack<Alloc ID2D1BitmapRenderTarget*> pool;

public:
	void Push(Alloc ID2D1BitmapRenderTarget* target) { 
		pool.push(target); 
	}
	Alloc ID2D1BitmapRenderTarget* Pop() {
		if (pool.empty()) { return nullptr; }
		Alloc ID2D1BitmapRenderTarget* target = pool.top();
		pool.pop();
		return target;
	}
	void Clear() {
		while (!pool.empty()) {
			pool.top()->Release();
			pool.pop();
		}
	}
	~TargetPool() { Clear(); }
};

class TileCache {
private:
	// The base target to create compatable bitmaps.
	Ref ID2D1RenderTarget* _compatable_target;

	TargetPool _target_pool;
public:
	// Only for child tiles to get the render target.
	Alloc ID2D1BitmapRenderTarget* BorrowRenderTarget();
	void ReturnRenderTarget(Ref ID2D1BitmapRenderTarget* target);

private:
	Rect _accessible_region;
	Rect _current_region;
	Size _tile_size;  // _tile_size can not be changed after the layer is created.

public:
	uint _background_color;
	Alloc ID2D1SolidColorBrush* _brush;

private:
	TileRange GetOverlappedTiles(Rect rect);

private:
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
	void Init(Ref ID2D1RenderTarget* compatable_target, Rect accessible_region, Rect current_region, Size tile_size);
	void Clear();
	bool IsEmpty() const;
	void SetCurrentRegion(Rect current_region);
	Rect GetCurrentRegion() const { return _current_region; }
	void DrawOn(ID2D1RenderTarget* target, Point position = point_zero);

public:
	void DrawColor(uint color, Rect rect);
};