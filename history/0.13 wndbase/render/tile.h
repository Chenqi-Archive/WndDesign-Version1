#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../figure/color.h"

// For drawing textbox.
#include "../style/textbox_style.h"  


BEGIN_NAMESPACE(WndDesign)

struct Target;
class TileCache;

// The tile that finally do the painting work.

class Tile {
private:
	Ref TileCache* _tile_cache;
	Ref Target* _target;   // May use a shared_pointer instead.
	bool IsAllocated() const { return _target != nullptr; }

private:
	friend class SystemWndLayer;
	// Called only when desktop creates a system layer.
	void SetSystemTarget(Ref TileCache* tile_cache, Ref Target* target) { 
		_tile_cache = tile_cache; _target = target;
	}

public:
	Tile(Ref TileCache* tile_cache);
	Tile(const Tile& target);
	~Tile();

	const Size GetSize() const;

	// Called by tile_cache to clear a tile with background.
	void Clear();

	// Draw myself on another tile, like a figure. For compositing.
	void DrawOn(Point point, uchar opacity, Tile& target_tile, Rect region) const ;

private:
	bool _has_begun;

	// The BeginDraw and EndDraw funcations are called by TileCache.
	friend class TileCache;
	void BeginDraw(Rect permitted_region);
	void EndDraw();

public:
	void DrawLine(Point start_point, Point end_point, uint width, Color32 color);
	void DrawRectangle(Rect region, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawColor(Rect region, Color32 color) { DrawRectangle(region, color_transparent, 0, color); }
	void DrawRoundedRectangle(Rect region, uint radius, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawEllipse(Rect region, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawText(Rect region, const wstring& string, const Ref TextBoxStyle& text_box_style);

	// Draw another target on myself. For compositing.
	void DrawTarget(Rect region, const Target& source_target, Point start_point_on_target, uchar opacity);

	//void DrawColor(Color32 color, Rect region = region_infinite);
	//void DrawLine(Point start, Point end, Color32 color, uchar width);
	//void DrawArc(Point center, uint radius, ushort degree_begin = 0, ushort degree_end = 360);
	//void DrawText(const Ref TextBoxStyle* textbox_style, Rect region = region_infinite);
};


END_NAMESPACE(WndDesign)