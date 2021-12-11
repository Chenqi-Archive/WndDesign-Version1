#include "tile.h"

Tile::Tile(TileCache* parent) {
	_parent = parent;
	_target = nullptr;
	_is_allocated = false;
}

Tile::Tile(const Tile& tile) {
	if (tile._is_allocated == true) {
		throw std::exception("You should only use the reference of a tile object!");
	}
	_parent = tile._parent;
	_target = tile._target;
	_is_allocated = tile._is_allocated;
}

Tile::~Tile() {
	if (_is_allocated) {
		_parent->ReturnRenderTarget(_target);
	}
}

void Tile::DrawColor(uint color, Rect rect) {
	if (!_is_allocated) {
		_target = _parent->BorrowRenderTarget();
	}
	_target->BeginDraw();

	// First clear the newly allocated target.
	if (!_is_allocated) {
		_target->Clear(D2D1::ColorF(_parent->_background_color));
		_is_allocated = true;
	}

	_parent->_brush->SetColor(D2D1::ColorF(color));
	_target->FillRectangle(D2D1::RectF(rect.point.x, rect.point.y, rect.RightBottom().x, rect.RightBottom().y),
						   _parent->_brush);

	_target->EndDraw();
}

void Tile::DrawOn(Rect region_on_tile, ID2D1RenderTarget* target, Point position) const {
	D2D1_RECT_F rect = D2D1::RectF(position.x,
								   position.y,
								   position.x + region_on_tile.size.width,
								   position.y + region_on_tile.size.height);
	if (!_is_allocated) {
		_parent->_brush->SetColor(D2D1::ColorF(_parent->_background_color));
		target->FillRectangle(rect, _parent->_brush);
	} else {
		ID2D1Bitmap* bitmap;
		_target->GetBitmap(&bitmap);
		target->DrawBitmap(bitmap,
						   rect,
						   1.0,
						   D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
						   D2D1::RectF(region_on_tile.point.x, region_on_tile.point.y,
						   region_on_tile.RightBottom().x, region_on_tile.RightBottom().y));
	}
}



Alloc ID2D1BitmapRenderTarget* TileCache::BorrowRenderTarget() {
	ID2D1BitmapRenderTarget* target;
	target = _target_pool.Pop();
	if (target == nullptr) {
		_compatable_target->CreateCompatibleRenderTarget({ (float)_tile_size.width, (float)_tile_size.height }, &target);
	}
	return target;
}

void TileCache::ReturnRenderTarget(Ref ID2D1BitmapRenderTarget* target) {
	if (target == nullptr) { return; }
	_target_pool.Push(target);
}

TileRange TileCache::GetOverlappedTiles(Rect rect) {
	Point begin_point;
	begin_point.x = div_floor(rect.point.x, _tile_size.width);
	begin_point.y = div_floor(rect.point.y, _tile_size.height);
	Point end_point;
	end_point.x = div_floor(rect.RightBottom().x - 1, _tile_size.width);
	end_point.y = div_floor(rect.RightBottom().y - 1, _tile_size.height);
	Size size = { end_point.x - begin_point.x + 1, end_point.y - begin_point.y + 1 };
	return { begin_point, size };
}

void TileCache::SwapOut() {
	auto it = _cache.begin();
	auto max_it = it;
	float max_distance = 0;
	TileRange tile_range = GetOverlappedTiles(_current_region);
	for (; it != _cache.end(); ++it) {
		float distance = Distance(ConvertLongToTileID(it->first), tile_range);
		if (distance > max_distance) {
			max_distance = distance;
			max_it = it;
		}
	}

	TileID tile = ConvertLongToTileID(max_it->first);
	debug_printf("Tile (%d, %d) was swapped out.\n", tile.x, tile.y);

	_cache.erase(max_it);

	// or calculate the distance between the tile rect and the current region.

	// or swap out all the tiles that have the common max_distance. Because the access
	//   of tiles are usually in group.
}

const Tile& TileCache::ReadTile(TileID tile_id) {
	auto it = _cache.find(ConvertTileIDToLong(tile_id));
	if (it != _cache.end()) {
		return it->second;
	} else {
		return _tile_read_only;
	}
}

Tile& TileCache::WriteTile(TileID tile_id) {
	auto it = _cache.find(ConvertTileIDToLong(tile_id));
	if (it != _cache.end()) {
		return it->second;
	}
	if (_cache.size() >= _max_capacity) {
		SwapOut();
	}

	// Another issue: when the _max_capacity has shrinked, there should be more tiles swapped out.


	it = _cache.insert(std::make_pair(ConvertTileIDToLong(tile_id), Tile(this))).first;
	
	TileID tile = ConvertLongToTileID(it->first);
	debug_printf("Tile (%d, %d) was inserted.\n", tile.x, tile.y);

	return it->second;
}

TileCache::TileCache() :_tile_read_only(this) {
	_compatable_target = nullptr;
	_brush = nullptr;
	_accessible_region = region_empty;
	_current_region = region_empty;
	_tile_size = size_max;
	_background_color = 0xFFFFFFFF; // White
	_max_capacity = 0;
}

TileCache::~TileCache() {
	SafeRelease(&_brush);
}

void TileCache::Init(Ref ID2D1RenderTarget* compatable_target, Rect accessible_region, Rect current_region, Size tile_size) {
	if (!IsEmpty()) { Clear(); }
	_compatable_target = compatable_target;
	compatable_target->CreateSolidColorBrush(D2D1::ColorF(_background_color), &_brush);
	_accessible_region = accessible_region;
	_current_region = _accessible_region.Intersect(current_region);
	_tile_size = tile_size;
	_max_capacity = GetOverlappedTiles({ { -1, -1 }, current_region.size }).Area();  // or enlarge?
}

void TileCache::Clear() {
	_compatable_target = nullptr;
	SafeRelease(&_brush);
	_accessible_region = region_empty;
	_current_region = region_empty;
	_tile_size = size_max;
	_cache.clear();
	_target_pool.Clear();
}

bool TileCache::IsEmpty() const { return _compatable_target == nullptr; }

void TileCache::SetCurrentRegion(Rect current_region) {
	_current_region = _accessible_region.Intersect(current_region);
	_max_capacity = GetOverlappedTiles({ { -1, -1 }, current_region.size }).Area();  // or enlarge?
}

void TileCache::DrawOn(ID2D1RenderTarget* target, Point position) {
	TileRange tiles = GetOverlappedTiles(_current_region);
	TileRangeIterator it(tiles);
	if (it.Finished()) { return; }
	target->BeginDraw();
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Point tile_origin = { tile_id.x * _tile_size.width, tile_id.y * _tile_size.height };
		Rect rect_on_tile = _current_region - (tile_origin - point_zero);  // coordinate space transformation.
		rect_on_tile = rect_on_tile.Intersect({ point_zero, _tile_size });
		const Tile& tile = ReadTile(tile_id);
		tile.DrawOn(rect_on_tile,
					target,
					rect_on_tile.point + (tile_origin - _current_region.point) + (position - point_zero));
	}
	target->EndDraw();
}

void TileCache::DrawColor(uint color, Rect rect) {
	TileRange tiles = GetOverlappedTiles(_current_region.Intersect(rect));
	TileRangeIterator it(tiles);
	for (; !it.Finished(); ++it) {
		TileID tile_id = it.Item();
		Point tile_origin = { tile_id.x * _tile_size.width, tile_id.y * _tile_size.height };
		Rect rect_on_tile = rect - (tile_origin - point_zero);  // coordinate space transformation.
		Tile& tile = WriteTile(tile_id);
		tile.DrawColor(color, rect_on_tile);
	}
}
