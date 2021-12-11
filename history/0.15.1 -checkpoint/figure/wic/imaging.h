#pragma once

#include "../../common/core.h"
#include "../../common/feature.h"
#include "../../geometry/geometry.h"
#include "../color.h"

#include <string>


///////////////////////////////////////////////////////////
////                 WIC Image Manager                 ////
///////////////////////////////////////////////////////////

// WIC resources.
struct IWICFormatConverter;
struct IWICBitmap;
struct IWICBitmapLock;

// Direct2D device-dependent resources.
struct ID2D1RenderTarget;
struct ID2D1Bitmap;


BEGIN_NAMESPACE(WndDesign)

using std::wstring;


class TargetResourceManager;


// The image resource based on Windows Image Component that can be loaded from file,
//   copyed, modified, and drawed.
class ImageResource : Uncopyable{
private:
	Alloc<IWICFormatConverter*> _wic_image;
	Alloc<IWICBitmap*> _wic_bitmap;
	Alloc<IWICBitmapLock*> _bitmap_data;

public:
	ImageResource(const wstring& file_name);
	ImageResource(void* file_address, size_t file_size);

	~ImageResource();

	bool IsEmpty() { return _wic_image == nullptr; }

	Size GetSize();

	// This function allows pixel-level copying and modification.
	// If called, the image will be converted to IWiCBitmap for more efficient access.
	Color GetColor(Point point);


	// For direct2d rendering.
private:
	Alloc<ID2D1Bitmap*> _d2d_bitmap;
	Ref<TargetResourceManager*> _resource_manager;
	void CreateD2DBitmap(Ref<ID2D1RenderTarget*> render_target);
	Ref<ID2D1Bitmap*> GetD2DBitmap();
};


END_NAMESPACE(WndDesign)