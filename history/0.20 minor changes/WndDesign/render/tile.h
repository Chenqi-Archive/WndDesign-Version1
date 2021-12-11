#pragma once

#include "../common/core.h"
#include "../common/feature.h"	
#include "../geometry/geometry.h"
#include "../figure/background.h"

BEGIN_NAMESPACE(WndDesign)

struct Target;
class TextLayout;

class TileCache;


class Tile : Uncopyable{
public:
	Tile(const TileCache& tile_cache, Size tile_size);
	~Tile();

private:
	const TileCache& _tile_cache;
	const Size _tile_size;
	Alloc<Target*> _target;

	bool IsAllocated() const { return _target != nullptr; }
	void Clear(); // Called by tile_cache to clear a tile with pure background, and the target will be released.

	// Tile size will only change for the read-only tile of tile_cache and the system tile.
	void SetTileSize(Size size) const { const_cast<Size&>(_tile_size) = size; }

private:
	// For system layer to draw figure on.
	friend class SystemWndLayer;
	void SetSystemTarget(Alloc<Target*> target) { _target = target; }

private:
	bool _has_begun;

	friend class TileCache;

	// Returns true if the tile active, or it will be added to the active tiles.
	bool BeginDraw(Rect permitted_region, Vector tile_offset, bool clear_with_background);
	void EndDraw();

	// Called by myself. Clear the region with background.
	void ClearWithBackground(Vector tile_offset, Rect region_on_tile);

public:
	// The drawing functions.
	// All drawing functions below blend the color with the original cleared color.

	void DrawColor(Rect region, Color color);
	void DrawImage(Rect region, const Image& image, Point start_point_on_image, uchar opacity, bool x_repeat, bool y_repeat);
	void DrawLine(Point start_point, Point end_point, uint width, Color color);
	void DrawRectangle(Rect region, Color border_color, uint border_width, const Background& background);
	void DrawRoundedRectangle(Rect region, uint radius, Color border_color, uint border_width, const Background& background);
	void DrawEllipse(Rect region, Color border_color, uint border_width, const Background& background);
	void DrawTextLayout(Rect region, const TextLayout& text_layout);

private:
	// Draw another target on myself. For compositing.
	void DrawTarget(Rect region, const Target& source_target, Point start_point_on_target, uchar opacity);
	// Draw myself on another tile as a figure. For compositing.
	void DrawOn(Vector my_offset, Point point, uchar opacity, Tile& target_tile, Rect region) const;
};


END_NAMESPACE(WndDesign)