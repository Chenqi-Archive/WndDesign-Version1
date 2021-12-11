#pragma once

#include "color.h"
#include "../geometry/geometry.h"

#include <string>
#include <memory>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::shared_ptr;


class ImageResource;

class Image {
private:
	shared_ptr<ImageResource> _image;

public:
	Image() :_image(nullptr) {}
	Image(const wstring& file_name);
	Image(void* file_address, size_t file_size);

	~Image() {}

	bool Load(const wstring& file_name);
	bool Load(void* file_address, size_t file_size);

	void Clear() { _image.reset(); }

	bool IsEmpty() const;

	const ImageResource& GetImageResource() const { return *_image; }
	const Size GetSize() const;

	Color GetColor(Point point) const ;
	bool IsOpaque(Point point) const { return GetColor(point).IsOpaque(); }
};


END_NAMESPACE(WndDesign)