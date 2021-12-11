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

	bool IsEmpty() const { return _wic_image == nullptr; }

	Size GetSize();

	// This function allows pixel-level copying and modification.
	// If called, the image will be converted to IWiCBitmap for more efficient access.
	Color GetColor(Point point);


	// For direct2d rendering.
private:
	Ref<TargetResourceManager*> _resource_manager;
	Alloc<ID2D1Bitmap*> _d2d_bitmap;
public:
	bool HasD2DBitmap() const { return _resource_manager != nullptr; }
	void CreateD2DBitmap(Ref<TargetResourceManager*> resource_manager);
	void DestroyD2DBitmap();
	void ClearD2DResource();  // Callback from TargetResourceManager.
	ID2D1Bitmap& GetD2DBitmap() const ;
};


END_NAMESPACE(WndDesign)