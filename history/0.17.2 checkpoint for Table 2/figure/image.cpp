#include "image.h"
#include "../common/exception.h"

#include "wic/imaging.h"

BEGIN_NAMESPACE(WndDesign)

using std::make_shared;


Image::Image(const wstring& file_name):
    _image(make_shared<ImageResource>(file_name)) {
    if (_image->IsEmpty()) { ExceptionDialog(L"Image is not loaded"); _image.reset(); }
}

Image::Image(void* file_address, size_t file_size) : 
    _image(make_shared<ImageResource>(file_address, file_size)){
    if (_image->IsEmpty()) { ExceptionDialog(L"Image is not loaded"); _image.reset(); }
}

bool Image::Load(const wstring& file_name) {
    _image = make_shared<ImageResource>(file_name);
    if (_image->IsEmpty()) { ExceptionDialog(L"Image is not loaded"); _image.reset(); return false; }
    return true;
}

bool Image::Load(void* file_address, size_t file_size) {
    _image = make_shared<ImageResource>(file_address, file_size);
    if (_image->IsEmpty()) { ExceptionDialog(L"Image is not loaded"); _image.reset(); return false; }
    return true;
}

bool Image::IsEmpty() const { 
    return _image == nullptr || _image->GetSize().IsEmpty(); 
}

const Size Image::GetSize() const {
    if (!_image) { ExceptionDialog(L"Image is not loaded"); return size_max; }
    return _image->GetSize();
}

Color Image::GetColor(Point point) const {
    if (!_image) { ExceptionDialog(L"Image is not loaded"); return ColorSet::White; }
    return _image->GetColor(point);
}


END_NAMESPACE(WndDesign)