#pragma once

#include "color.h"
#include "image.h"


BEGIN_NAMESPACE(WndDesign)


// You can set either color or image for background. Similar to that of css styles, 
//   image could also be repeated at x or y axis.
// If image is not repeated, the rest space will be filled with color.
// The size of background is determined by window, if the size is extra large, like in a scrollwnd,
//   the image will be automatically repeated.
class Background {
private:
	Color _color;
	Image _image;
	bool _x_repeat;
	bool _y_repeat;

public:
	explicit constexpr Background(Color color = color_transparent) :
		_color(color), _image(), _x_repeat(true), _y_repeat(true) {
	}

	// User functions.
	constexpr void setColor(Color color) { _color = color;}
	void setImage(const Image& image, bool x_repeat = true, bool y_repeat = true) {
		_image = image;
		_x_repeat = x_repeat;
		_y_repeat = y_repeat;
	}
	void setImage(const wstring& image_file, bool x_repeat = true, bool y_repeat = true) {
		_image.Load(image_file);
		_x_repeat = x_repeat;
		_y_repeat = y_repeat;
	}

	
	// Used by tile for drawing.
	bool IsSolidColor() const { return _image.IsEmpty(); }
	const Color GetColor() const { return _color; }

	bool HasImage() const { return !_image.IsEmpty(); }
	const Image& GetImage() const { return _image; }
	const bool IsXRepeat() const { return _x_repeat; }
	const bool IsYRepeat() const { return _y_repeat; }

	bool IsEmpty() const { return GetColor().alpha == 0 && !HasImage(); }

	// Used by window to test if a point is opaque.
	bool IsOpaque(Point point) const {
		if (_image.IsEmpty()) { return _color.IsOpaque(); }

		auto mod_as_unsigned = [](int val, uint length) -> uint {
			if (val >= 0) { return static_cast<uint>(val) % length; }
			int remainder = val % static_cast<int>(length);
			return static_cast<uint>(remainder == 0 ? remainder : remainder + static_cast<int>(length));
		};

		uint x, y;
		Size image_size = _image.GetSize();
		if (_x_repeat) {
			x = mod_as_unsigned(point.x, image_size.width);
		} else {
			x = static_cast<uint>(point.x); // If point.x is negative, x will be extremely large.
		}
		if (_y_repeat) {
			y = mod_as_unsigned(point.y, image_size.height);
		} else {
			y = static_cast<uint>(point.y);
		}
	
		if (x < image_size.width && y < image_size.height) {
			return _image.IsOpaque(point);
		}
		return _color.IsOpaque();
	}
};


static const Background background_transparent = Background();


END_NAMESPACE(WndDesign)