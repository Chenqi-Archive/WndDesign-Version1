#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"

BEGIN_NAMESPACE(WndDesign)

struct WndStyle {
	// Size
	Size size = { 800px, 500px };
	Size max_size = infinite_size;
	Size min_size = { 200px, 200px };

	// Position
	Point left_top = { 100, 100 };  // Negative value allowed.
	ushort rotate = 0deg;   // Clockwise rotation at left_top

	// The hint for rendering.
	// As a subwindow, if the position or size may change, or the window
	//   may disappear or be added after the parent window has began rendering,
	//   then set the style true, or the change will be discarded.
	// These styles have no effect on a system window.
	bool hint_position_change = false;  
	bool hint_size_change = false;		// Hint for allocating render target.
	bool hint_remove = false;		// Will or not be destroyed or removed after displaying.
	bool hint_repaint = false;       

	// Border
	uchar border_width = 3px;
	const Ref SolidColor* border_color = &built_in_resources.color_black;

	// Background
	const Ref Texture* background = &built_in_resources.color_white;

	// Effect
	uchar opacity = 0xFF;
	// Filter* filter;

	bool mouse_penetrate = false;  // : 1
};

// If you want to implement filter effect, just create a wnd and set the effect.



END_NAMESPACE(WndDesign)
