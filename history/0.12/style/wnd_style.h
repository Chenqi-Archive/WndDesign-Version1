#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"

BEGIN_NAMESPACE(WndDesign)

constexpr uint operator""px(unsigned long long number) {
	return static_cast<uint>(number);
}
constexpr uint operator""pct(unsigned long long number) {
	return static_cast<uint>(number) | 0x40000000;
}

constexpr int position_auto = 0x80000000;
constexpr int position_center = 0xF0000000;


// The style for painting inside the parent window.
// Parent window parses the style and provide canvases for child windows.

struct WndStyle {
	//////////////////////////////////////////////////////////////////////
	//// Styles used by parent windows: size, position, render styles ////
	//////////////////////////////////////////////////////////////////////
	
	// The size of the window.
	struct SizeStyle {   // 24 bytes
		Size normal = size_auto;
		Size min = size_min;
		Size max = size_max;
		void Set(Size normal, Size min = size_min, Size max = size_max) { this->normal = normal; this->min = min; this->max = max; }
		void SetFixed(Size size) { normal = min = max = size; }  // The window could not resize.
	}size;

	// The margin between the border and the parent window.
	struct PositionStyle {    // 16 bytes
		int left = position_auto;
		int top = position_auto;
		int right = position_auto;
		int down = position_auto;
		void Set(int left, int top, int right, int down) { this->left = left; this->top = top; this->right = right; this->down = down; }
		PositionStyle& SetHorizontalCenter() { left = right = position_center; return *this; }
		PositionStyle& SetVerticalCenter() { top = down = position_center; }
	}position;

	// The hint for rendering.
	// These render hints have no effect on a system window.
	struct RenderHint {	   // 1 byte
		bool change_position : 1;			// The window's position may change.
		bool change_size : 1;				// The window's size may change.
		bool destroy : 1;					// The window may be removed or readded to the parent.
		bool redraw : 1;					// The window may redraw its contents.
		bool background_transparent : 1;	// The background may be transparent somewhere. 
											//   It will automatically be true if you have set the opacity less than 255.
		RenderHint() { change_position = false; change_size = false; destroy = false; redraw = false; background_transparent = false; }
	}hint;

	// For subwindows of scroll window only. If set, the window will be fixed at top.
	bool is_fixed : 1; 

	// Mouse penetrate for opaque regions that has non-255 opacity.
	bool mouse_penetrate_content : 1;
	// Mouse penetrate for background regions that has non-255 opacity.
	bool mouse_penetrate_background : 1;

	// Effect
	uchar opacity = 0xFF;   // The opacity of all contents. If there are already transparent contents, use the smaller value.
	uchar blur_radius = 0px;   // The gaussian blur radius for overlapped content. Only useful when transparent.
	// Filter* filter;

	////////////////////////////////////////////////////////////////////////////
	//// Styles used by the window itself: background, border, other styles ////
	////////////////////////////////////////////////////////////////////////////

	// Background.
	enum class BackgroundRepeat : uchar { 
		None, 
		Horizontal, 
		Vertical, 
		Repeat 
	} background_repeat = BackgroundRepeat::Repeat;
	const Ref Texture* background = &built_in_resources.color_white;

	// Border
	struct BorderStyle {    // 12 bytes
		uint width = 0px;
		Color32 color = ColorSet::Black;
		uint radius = 0px;	// Rounded angle
		void Set(uint width, Color32 color, uint radius = 0px) { this->width = width; this->color = color; this->radius = radius; }
	}border;

	//// Shadow // Not supported.
	//struct ShadowStyle {
	//	char horizontal;
	//	char vertical;
	//	uchar blur;
	//} shadow;

	WndStyle() {
		is_fixed = false;
		mouse_penetrate_content = false;
		mouse_penetrate_background = true;
	}
};


END_NAMESPACE(WndDesign)
