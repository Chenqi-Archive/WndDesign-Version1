#include "tile.h"
#include "tile_cache.h"

#include "direct2d/factory.h"
#include "direct2d/target.h"
#include "direct2d/dwrite.h"

#undef Alloc
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")

BEGIN_NAMESPACE(WndDesign)


// Shrink a rect region by length.
void ShrinkRegion(Rect& region, uint length) {
	uint max_length = min(region.size.width, region.size.height) / 2;
	if (length > max_length) { length = max_length; }
	region.point.x += static_cast<int>(length);
	region.point.y += static_cast<int>(length);
	region.size.width -= 2 * length;
	region.size.height -= 2 * length;
}


inline D2D1_RECT_F Rect2RECT(Rect rect) {
	return D2D1::RectF(static_cast<FLOAT>(rect.point.x), static_cast<FLOAT>(rect.point.y), 
					   static_cast<FLOAT>(rect.RightBottom().x), static_cast<FLOAT>(rect.RightBottom().y));
}

inline Rect RECT2Rect(D2D1_RECT_F rect) {
	Rect _rect;
	_rect.point.x = static_cast<int>(rect.left);
	_rect.point.y = static_cast<int>(rect.top);
	_rect.size.width = static_cast<uint>(static_cast<int>(rect.right) - static_cast<int>(rect.left));
	_rect.size.height = static_cast<uint>(static_cast<int>(rect.bottom) - static_cast<int>(rect.top));
	return _rect;
}

inline D2D1_POINT_2F Point2POINT(Point point) {
	return D2D1::Point2F(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
}

inline float OpacityToFloat(uchar opacity) {
	return opacity / (float)0xFF;
}

inline D2D1_COLOR_F Color2COLOR(Color color) {
	return D2D1::ColorF(color, OpacityToFloat(color.alpha));
}


Tile::Tile(Ref<TileCache*> tile_cache) :
	_tile_cache(tile_cache),
	_target(nullptr),
	_has_begun(false){
}

Tile::~Tile() {
	Clear();
}

const Size Tile::GetSize() const {
	return _tile_cache->GetTileSize();
}

void Tile::Clear() {
	// Return the target.
	if (IsAllocated() && _tile_cache != nullptr) {
		_tile_cache->ReturnTarget(_target);
		_target = nullptr;
		_has_begun = false;
	}
}

void Tile::DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const {
	if (IsAllocated()) {
		target_tile.DrawTarget(region, *_target, point, opacity);
	} else {
		// Has no target, draw the default background directly.
		// May use a figure as the background later.
		Color background = _tile_cache->GetBackground();
		target_tile.DrawColor(region, Color(background, background.alpha * opacity / 0xFF));
	}
}


bool Tile::BeginDraw(Rect permitted_region) {
	if (_has_begun) { 
		// Already begun, reset the permitted region.
		ID2D1RenderTarget& target = _target->GetTarget();
		target.PopAxisAlignedClip();
		target.PushAxisAlignedClip(Rect2RECT(permitted_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		return false;
	}

	bool need_clear = false;
	if (!IsAllocated()) {
		_target = _tile_cache->UseTarget();
		need_clear = true;
	}

	ID2D1RenderTarget& target = _target->GetTarget();

	target.BeginDraw();
	_has_begun = true;

	// First clear the newly allocated target.
	if (need_clear) {
		target.Clear(Color2COLOR(_tile_cache->GetBackground()));
	}

	// Set the permitted region.
	target.PushAxisAlignedClip(Rect2RECT(permitted_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	return true;
}

void Tile::EndDraw() {
	if (!_has_begun) { return; }

	ID2D1RenderTarget& target = _target->GetTarget();

	// Remove the permitted region.
	target.PopAxisAlignedClip();

	int res = target.EndDraw();
	if (res == D2DERR_RECREATE_TARGET) {
		_tile_cache->GetResourceManager()->RecreateResources();
	}

	_has_begun = false;
}

void Tile::DrawLine(Point start_point, Point end_point, uint width, Color color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache->GetResourceManager()->UseSolidColorBrush();
	brush.SetColor(Color2COLOR(color));
	target.DrawLine(Point2POINT(start_point), Point2POINT(end_point),&brush, static_cast<FLOAT>(width));
}

void Tile::DrawRectangle(Rect region, Color border_color, uint border_width, Color fill_color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache->GetResourceManager()->UseSolidColorBrush();
	if (fill_color.alpha > 0) {
		brush.SetColor(Color2COLOR(fill_color));
		target.FillRectangle(Rect2RECT(region), &brush);
	}
	if (border_width > 0) {
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawRectangle(Rect2RECT(region), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawRoundedRectangle(Rect region, uint radius, Color border_color, uint border_width, Color fill_color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache->GetResourceManager()->UseSolidColorBrush();
	if (fill_color.alpha > 0) {
		brush.SetColor(Color2COLOR(fill_color));
		target.FillRoundedRectangle(D2D1::RoundedRect(Rect2RECT(region), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), &brush);
	}
	if (border_width > 0) {
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawRoundedRectangle(D2D1::RoundedRect(Rect2RECT(region), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawEllipse(Rect region, Color border_color, uint border_width, Color fill_color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache->GetResourceManager()->UseSolidColorBrush();
	if (fill_color.alpha > 0) {
		brush.SetColor(Color2COLOR(fill_color));
		target.FillEllipse(D2D1::Ellipse(Point2POINT(region.Center()), static_cast<FLOAT>(region.size.width / 2), static_cast<FLOAT>(region.size.height / 2)), &brush);
	}
	if (border_width > 0) {
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawEllipse(D2D1::Ellipse(Point2POINT(region.Center()), static_cast<FLOAT>(region.size.width / 2), static_cast<FLOAT>(region.size.height / 2)), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawTextLayout(Rect region, const TextLayout& text_layout) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache->GetResourceManager()->UseSolidColorBrush();
	brush.SetColor(Color2COLOR(text_layout._style.font._color));
	target.DrawTextLayout(Point2POINT(region.point), text_layout._layout, &brush,
						   D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
}

void Tile::DrawTarget(Rect region, const Target& source_target, Point start_point_on_target, uchar opacity) {
	ID2D1RenderTarget& target = _target->GetTarget();
	Ref<ID2D1Bitmap*> bitmap;
	// The target that can draw on other target can not be a HWND target.
	static_cast<ID2D1BitmapRenderTarget&>(source_target.GetTarget()).GetBitmap(&bitmap);
	target.DrawBitmap(bitmap,
					  Rect2RECT(region),
					  OpacityToFloat(opacity),
					  D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
					  Rect2RECT(Rect(start_point_on_target, region.size)));
}


END_NAMESPACE(WndDesign)