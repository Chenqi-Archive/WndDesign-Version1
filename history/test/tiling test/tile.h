#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <unordered_map>

#define Ref 
#define Alloc 



using uint = unsigned int;
using Size = struct { uint width; uint height; };
using Point = struct { int x; int y; };
using Rect = struct { Point point; Size size; };

inline int div_floor(int a, uint b) {
	if (a >= 0) { return a / b; }
	else { return (a - b + 1) / b; }
}
inline uint mod_floor(int a, uint b) {
	int res = a % b;
	if (res < 0) { return res + b; }
	return res;
}

class TileCache {
private:
	// The base target to create compatable bitmaps.
	Ref ID2D1RenderTarget* _compatable_target;

private:
	Size _whole_size;
	Size _tile_size;

	Rect _visible_region;

	using TileID = Size;

private:
	std::unordered_map<TileID, Alloc ID2D1BitmapRenderTarget*> _tiles;
	Alloc ID2D1SolidColorBrush* _brush;

	Ref ID2D1BitmapRenderTarget* GetTile(TileID tile_id) {
		Ref Alloc ID2D1BitmapRenderTarget* target;
		auto it = _tiles.find(tile_id);
		if (it == _tiles.end()) {
			_compatable_target->CreateCompatibleRenderTarget(D2D1::SizeF(_tile_size.width, _tile_size.height), &target);
			_tiles[tile_id] = target;
		} else {
			target = it->second;
		}
		return target;
	}

public:
	TileCache() {
		_compatable_target = nullptr;
		_brush = nullptr;
		_whole_size = {};
		_tile_size = { 1,1 };
		_visible_region = {};
	}
	~TileCache() {
		for (auto it : _tiles) {
			it.second->Release();
		}
		_brush->Release();
	}
	void Init(Ref ID2D1RenderTarget* compatable_target, Size whole_size, Size tile_size) {
		_compatable_target = compatable_target;
		_whole_size = whole_size;
		_tile_size = tile_size;
		_visible_region = { {0,0}, _whole_size };
		compatable_target->CreateSolidColorBrush(D2D1::ColorF(0), &_brush);
	}
	void Clear() {
		for (auto it : _tiles) {
			it.second->Release();
		}
		_tiles.clear();
		_brush->Release();
	}
	bool IsEmpty() const {return _tiles.empty();}
	void SetSize(Size size) {
		_whole_size = size;
		_visible_region = { {0,0}, _whole_size };
	}
	Size GetSize() const { return _whole_size; }

	void DrawOn(ID2D1RenderTarget* target) {
		D2D1_SIZE_F sizef = target->GetSize();
		Size size = { sizef.width, sizef.height };
		
		target->BeginDraw();
		int x0 = 0, y0 = 0;
		int x = 0, y = 0;
		for (; y0 < size.height; y0 += _tile_size.height, y++) {
			for (; x0 < size.width; x0 += _tile_size.width, x++) {
				ID2D1Bitmap* bitmap;
				GetTile(TileID{ x, y })->GetBitmap(&bitmap);
				target->DrawBitmap(bitmap,
								   D2D1::RectF(x0, y0, x0 + _tile_size.width, y0 + _tile_size.height),
								   1.0,
								   D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
								   D2D1::RectF(0, 0, _tile_size.width, _tile_size.height));
			}
		}
		target->EndDraw();
	}

public:
	void DrawColor(uint color, Rect rect) {
		int rdx = rect.point.x + rect.size.width;
		int rdy = rect.point.y + rect.size.height;
		int x0 = div_floor(rect.point.x, _tile_size.width);
		int y0 = div_floor(rect.point.y, _tile_size.height);
		int x = mod_floor(rect.point.x, _tile_size.width);
		int y = mod_floor(rect.point.y, _tile_size.height);

		_brush->SetColor(D2D1::ColorF(color));

		for (; y0 * _tile_size.height < rdy; y0++, ) {
			for (; x0 * _tile_size.width < rdx; x0++, x-=_tile_size.width) {
				Ref ID2D1BitmapRenderTarget* target = GetTile({ x0, y0 });
				if (target == nullptr) {

				}
				target->BeginDraw();
				target->FillRectangle(D2D1::RectF(x, y, x + rect.size.width, y + rect.size.height), _brush);
				target->EndDraw();
			}
		}
	}
};