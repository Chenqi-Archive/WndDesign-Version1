#pragma once

#include "color.h"
#include "../geometry/geometry.h"

#include <string>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::shared_ptr;


class ImageResource;

// An image . Similar to brushes in D2D.
class Bitmap {
private:
	shared_ptr<ImageResource> _image;

public:
	Bitmap() :_image(nullptr) {}
	~Bitmap() {}

	bool Load(Color color);
	bool Load(const wstring& file_name);
	bool Load(void* file_address, size_t file_size);

	Size GetSize();

	Color GetColor(Point point);
	bool IsOpaque(Point point) { return GetColor(point).IsOpaque(); }
};


END_NAMESPACE(WndDesign)