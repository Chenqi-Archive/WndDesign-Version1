#include "image.h"

#include "wic/imaging.h"

BEGIN_NAMESPACE(WndDesign)


bool Image::Load(Color color) {
    _color = color;
    _image.reset();
    return false;
}

bool Image::Load(const wstring& file_name) {
    return false;
}

bool Image::Load(void* file_address, size_t file_size) {
    return false;
}

bool Image::Create(Size size) {
    return false;
}

Size Image::GetSize() {
    if (!_image) { return size_max; }  // Pure color.
    return _image
}

Color Image::GetColor(Point point) {
    return Color();
}


END_NAMESPACE(WndDesign)