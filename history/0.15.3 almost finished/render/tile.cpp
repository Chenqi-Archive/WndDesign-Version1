#include "tile.h"
#include "tile_cache.h"

#include "../common/exception.h"

#include "direct2d/factory.h"
#include "direct2d/target.h"
#include "direct2d/dwrite.h"

#include "../figure/wic/imaging.h"

#include <d2d1.h>

BEGIN_NAMESPACE(WndDesign)

using ImageRange = Rect;
// For calculating image repeat ranges.
inline ImageRange RegionToImageRange(Rect region, Size image_size) {
	auto div_floor = [](int a, int b) -> int { if (a >= 0) { return a / b; } else { return (a - b + 1) / b; } };
	auto div_ceil = [](int a, int b) -> int { if (a <= 0) { return a / b; } else { return (a + b - 1) / b; } };
	Point begin_point;
	begin_point.x = div_floor(region.point.x, image_size.width);
	begin_point.y = div_floor(region.point.y, image_size.height);
	Point end_point;
	end_point.x = div_ceil(region.RightBottom().x, image_size.width);
	end_point.y = div_ceil(region.RightBottom().y, image_size.height);
	Size size = Size(static_cast<uint>(end_point.x - begin_point.x), static_cast<uint>(end_point.y - begin_point.y));
	return ImageRange(begin_point, size);
}
inline Point GetImageOrigin(Point image_repeat, Size image_size) {
	return Point(image_repeat.x * static_cast<int>(image_size.width), image_repeat.y * static_cast<int>(image_size.height));
}
// Iterate the points rowwise inside the rect.
class RectPointIterator {
private:
	Point _start, _end;
	Point _current_point;
public:
	RectPointIterator(Rect rect) : _start(rect.LeftTop()), _end(rect.RightBottom()), _current_point(_start) {}
	void Next() {
		if (Finished()) { Error(); } _current_point.x++;
		if (_current_point.x >= _end.x) { _current_point.x = _start.x; _current_point.y++; }
	}
	void operator++() { Next(); }
	bool Finished() { return _current_point.x >= _end.x || _current_point.y >= _end.y; }
	Point Item() const { return _current_point; }
};


// Shrink a rect region by length.
inline void ShrinkRegion(Rect& region, uint length) {
	uint max_length = min(region.size.width, region.size.height) / 2;
	if (length > max_length) { length = max_length; }
	region.point.x += static_cast<int>(length);
	region.point.y += static_cast<int>(length);
	region.size.width -= 2 * length;
	region.size.height -= 2 * length;
}

inline Color BlendColorWithOpacity(Color color, uchar opacity) {
	return Color(color, color.alpha * opacity / 0xFF);
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

inline ID2D1Brush& GetBackgroundBrush(TargetResourceManager& resource_manager, const Background& bitmap) {
	if (bitmap.IsSolidColor()) {
		ID2D1SolidColorBrush& solid_color_brush = resource_manager.UseSolidColorBrush();
		solid_color_brush.SetColor(Color2COLOR(bitmap.GetColor()));
		return solid_color_brush;
	} else {
		ID2D1BitmapBrush& bitmap_brush = resource_manager.UseBitmapBrush(bitmap.GetImage().GetImageResource());
		bitmap_brush.SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
		bitmap_brush.SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
	#pragma message("The image is drawn repeatedly regardless of the background settings.")
		return bitmap_brush;
	}
}



Tile::Tile(const TileCache& tile_cache) :
	_tile_cache(tile_cache),
	_target(nullptr),
	_has_begun(false){
}

Tile::~Tile() {
	Clear();
}

const Size Tile::GetSize() const {
	return _tile_cache.GetTileSize();
}

void Tile::Clear() {
	// Return the target.
	if (IsAllocated()) {
		EndDraw();
		_tile_cache.ReturnTarget(_target);
		_target = nullptr;
	}
}

bool Tile::BeginDraw(Rect permitted_region, Vector tile_offset, bool clear_with_background) {
	if (_has_begun) { 
		// Already begun, reset the permitted region.
		ID2D1RenderTarget& target = _target->GetTarget();
		target.PopAxisAlignedClip();
		target.PushAxisAlignedClip(Rect2RECT(permitted_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		return true;
	}

	bool need_clear = false;
	if (!IsAllocated()) {
		_target = _tile_cache.UseTarget();
		need_clear = true;
	}

	ID2D1RenderTarget& target = _target->GetTarget();
	target.BeginDraw();
	_has_begun = true;

	if (need_clear) {
		ClearWithBackground(tile_offset, Rect(point_zero, _tile_cache.GetTileSize()));
	}

	// Set the permitted region.
	target.PushAxisAlignedClip(Rect2RECT(permitted_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	if (clear_with_background && !need_clear) {
		ClearWithBackground(tile_offset, permitted_region);
	}

	return false;
}

void Tile::EndDraw() {
	if (!_has_begun) { return; }

	ID2D1RenderTarget& target = _target->GetTarget();

	// Remove the permitted region.
	target.PopAxisAlignedClip();

	int res = target.EndDraw();
	if (res == D2DERR_RECREATE_TARGET) {
		_tile_cache.GetResourceManager()->RecreateResources();
	}

	_has_begun = false;
}

void Tile::ClearWithBackground(Vector tile_offset, Rect region_on_tile) {
	ID2D1RenderTarget& target = _target->GetTarget();
	const Background& background = _tile_cache.GetBackground();
	target.Clear(Color2COLOR(background.GetColor()));
	if (background.HasImage()) {
		DrawImage(region_on_tile, background.GetImage(), region_on_tile.point + tile_offset,
				  0xFF, background.IsXRepeat(), background.IsYRepeat());
	}
}

void Tile::DrawColor(Rect region, Color color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
	brush.SetColor(Color2COLOR(color));
	target.FillRectangle(Rect2RECT(region), &brush);
}

void Tile::DrawImage(Rect region, const Image& image, Point start_point_on_image, uchar opacity, bool x_repeat, bool y_repeat) {
	if (image.IsEmpty()) { return; }

	Size image_size = image.GetSize();
	// Calculate the image range that overlaps with the tile region.
	Rect region_on_tile = region.Intersect(Rect(point_zero, _tile_cache.GetTileSize()));
	Vector tile_to_image_conversion = start_point_on_image - region.point;
	Rect region_on_image = region_on_tile + tile_to_image_conversion;
	ImageRange image_range = RegionToImageRange(region_on_image, image_size);
	ImageRange repeat_range = region_infinite;
	if (x_repeat) { repeat_range.point.x = 0; repeat_range.size.width = 1; }
	if (y_repeat) { repeat_range.point.y = 0; repeat_range.size.height = 1; }
	image_range = image_range.Intersect(repeat_range);
	if(image_range.IsEmpty()) { return; }

	// Draw every single image repeat on the tile.
	const ImageResource& image_resource = image.GetImageResource();
	if (!image_resource.HasD2DBitmap()) { const_cast<ImageResource&>(image_resource).CreateD2DBitmap(_tile_cache.GetResourceManager()); }
	ID2D1Bitmap& bitmap = image_resource.GetD2DBitmap();
	ID2D1RenderTarget& target = _target->GetTarget();
	for (RectPointIterator it(image_range); !it.Finished(); ++it) {
		Point image_origin = GetImageOrigin(it.Item(), image_size);
		Rect image_region_on_tile = Rect(image_origin, image_size) - tile_to_image_conversion;
		Rect drawing_region_on_tile = region_on_tile.Intersect(image_region_on_tile);
		Rect drawing_region_of_image = drawing_region_on_tile - (image_region_on_tile.point - point_zero);
		target.DrawBitmap(&bitmap,
						  Rect2RECT(drawing_region_on_tile),
						  OpacityToFloat(opacity),
						  D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
						  Rect2RECT(drawing_region_of_image));
	}
}

void Tile::DrawLine(Point start_point, Point end_point, uint width, Color color) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
	brush.SetColor(Color2COLOR(color));
	target.DrawLine(Point2POINT(start_point), Point2POINT(end_point),&brush, static_cast<FLOAT>(width));
}

void Tile::DrawRectangle(Rect region, Color border_color, uint border_width, const Background& background) {
	ID2D1RenderTarget& target = _target->GetTarget();
	if (!background.IsEmpty()) {
		DrawColor(region, background.GetColor());
		if (background.HasImage()) {
			DrawImage(region, background.GetImage(), point_zero,
					  0xFF, background.IsXRepeat(), background.IsYRepeat());
		}
	}
	if (border_width > 0) {
		ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawRectangle(Rect2RECT(region), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawRoundedRectangle(Rect region, uint radius, Color border_color, uint border_width, const Background& background) {
	ID2D1RenderTarget& target = _target->GetTarget();
	if (!background.IsEmpty()) {
		ID2D1Brush& brush = GetBackgroundBrush(*_tile_cache.GetResourceManager(), background);
		target.FillRoundedRectangle(D2D1::RoundedRect(Rect2RECT(region), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), &brush);
	}
	if (border_width > 0) {
		ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawRoundedRectangle(D2D1::RoundedRect(Rect2RECT(region), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawEllipse(Rect region, Color border_color, uint border_width, const Background& background) {
	ID2D1RenderTarget& target = _target->GetTarget();
	if (!background.IsEmpty()) {
		ID2D1Brush& brush = GetBackgroundBrush(*_tile_cache.GetResourceManager(), background);
		target.FillEllipse(D2D1::Ellipse(Point2POINT(region.Center()), static_cast<FLOAT>(region.size.width / 2), static_cast<FLOAT>(region.size.height / 2)), &brush);
	}
	if (border_width > 0) {
		ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
		ShrinkRegion(region, border_width / 2); // Shrink the region so that border will be drawn inside.
		brush.SetColor(Color2COLOR(border_color));
		target.DrawEllipse(D2D1::Ellipse(Point2POINT(region.Center()), static_cast<FLOAT>(region.size.width / 2), static_cast<FLOAT>(region.size.height / 2)), &brush, static_cast<FLOAT>(border_width));
	}
}

void Tile::DrawTextLayout(Rect region, const TextLayout& text_layout) {
	ID2D1RenderTarget& target = _target->GetTarget();
	ID2D1SolidColorBrush& brush = _tile_cache.GetResourceManager()->UseSolidColorBrush();
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


void Tile::DrawOn(Vector my_offset, Point point, uchar opacity, Tile& target_tile, Rect region) const {
	if (IsAllocated()) {
		target_tile.DrawTarget(region, *_target, point, opacity);
	} else {
		// My target has not allocated, draw the background directly on the target_tile.
		const Background& background = _tile_cache.GetBackground();
		target_tile.DrawColor(region, BlendColorWithOpacity(background.GetColor(), opacity));
		if (background.HasImage()) {
			target_tile.DrawImage(region, background.GetImage(), point + my_offset,
								  opacity, background.IsXRepeat(), background.IsYRepeat());
		}
	}
}


END_NAMESPACE(WndDesign)