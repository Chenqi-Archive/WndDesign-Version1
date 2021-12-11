#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../texture/texture.h"

#include "render_style.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

// A wrapper for RenderTargetBase with a rect region.

class RenderTargetBase;

class RenderTarget {
	Ref RenderTargetBase* _base;
	Rect _region;

public:
	// Create a new RenderTarget inside the current render target(the intersection), 
	//   the overflow region will be disgarded.
	RenderTarget CreateRenderTarget(const Rect& sub_region) const;


	// The region should be the bounded rectangle of all the successive draw regions.
	bool BeginDraw(const Rect& region = full_region) const ;
	void EndDraw() const;

	// Styles

	void DrawRenderTarget(const Rect& region, const Ref RenderTargetBase* target_base);  // Should check the content.
	void DrawText(const Rect& region, const Ref TextBoxStyle* textbox_style);
	void DrawSolidColor(const Rect& region, const Ref SolidColor* solid_color);
	void DrawBitmap(const Rect& region, const Ref Bitmap* bitmap);
};

END_NAMESPACE(WndDesign)