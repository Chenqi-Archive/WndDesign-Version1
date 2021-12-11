#pragma once

#include "../common/core.h"
#include "../common/feature.h"	
#include "../geometry/geometry.h"
#include "../figure/color.h"


BEGIN_NAMESPACE(WndDesign)

struct Target;
class ImageSource;
class TextLayout;

class TileCache;


class Tile : Uncopyable{
public:
	Tile(Ref<TileCache*> tile_cache, Vector offset, Size size);
	~Tile();

private:
	Ref<Target*> _target;   // May use a shared_pointer instead.
	Ref<TileCache*> _tile_cache;
	Vector _offset;
	Size _size;

	bool IsAllocated() const { return _target != nullptr; }
	void Clear(); // Also called by tile_cache to clear a tile with pure background.
	const Size GetSize() const { return _size; }

private:
	// Only for desktop to create a system layer.
	friend class SystemWndLayer;
	void SetSystemTarget(Ref<TileCache*> tile_cache, Ref<Target*> target) {
		_tile_cache = tile_cache; _target = target;
	}

private:
	bool _has_begun;

	// Return true if it is was not active, so it will be added to the active tiles.
	friend class TileCache;
	bool BeginDraw(Rect permitted_region);
	void EndDraw();

public:
	void DrawLine(Point start_point, Point end_point, uint width, Color color);
	void DrawRectangle(Rect region, Color border_color, uint border_width, Color fill_color);
	void DrawColor(Rect region, Color color) { DrawRectangle(region, color_transparent, 0, color); }
	void DrawRoundedRectangle(Rect region, uint radius, Color border_color, uint border_width, Color fill_color);
	void DrawEllipse(Rect region, Color border_color, uint border_width, Color fill_color);
	void DrawImage(Rect region, const ImageSource& image, );
	void DrawTextLayout(Rect region, const TextLayout& text_layout);

private:
	// Draw another target on myself. For compositing.
	void DrawTarget(Rect region, const Target& source_target, Point start_point_on_target, uchar opacity);
	// Draw myself on another tile like a figure. For compositing.
	void DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const;
};


END_NAMESPACE(WndDesign)