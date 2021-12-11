#include "imaging.h"
#include "../../common/exception.h"

#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

BEGIN_NAMESPACE(WndDesign)


template<class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { std::abort(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)


static const wstring load_file_exception = L"Load File Error";


class WicFactory {
private:
	IWICImagingFactory* factory;
	WicFactory() :factory(NULL) {
		// Initialize COM
		CoInitialize(NULL);
		// Create the COM imaging factory
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&factory)
		);
		AbortWhenFailed(hr);
	}
	~WicFactory() {
		SafeRelease(&factory);
	}

private:
	static Alloc<IWICFormatConverter*> LoadFromDecoder(IWICBitmapDecoder* decoder) {
		IWICBitmapFrameDecode* source = NULL;
		IWICFormatConverter* converter = NULL;

		auto hr = decoder->GetFrame(0, &source);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		// The format of source might not be compatable with direct2d, must convert it to 32bppPBGRA.
		hr = WicFactory::Get()->CreateFormatConverter(&converter);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		hr = converter->Initialize(
			source,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		SafeRelease(&source);
		return converter;

	Exception:
		SafeRelease(&converter);
		SafeRelease(&source);
		return nullptr;
	}

public:
	static Ref<IWICImagingFactory*> Get() {
		static WicFactory factory;
		return factory.factory;
	}

	static Alloc<IWICFormatConverter*> Load(const wstring& file_name) {
		IWICBitmapDecoder* decoder = NULL;
		IWICFormatConverter* converter = NULL;

		auto hr = Get()->CreateDecoderFromFilename(
			file_name.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		converter = LoadFromDecoder(decoder);
		if (converter == nullptr) { goto Exception; }

	Exception:
		SafeRelease(&decoder);
		return converter;
	}

	static Alloc<IWICFormatConverter*> Load(void* file_address, size_t file_size) {
		IWICStream* stream = NULL;
		IWICBitmapDecoder* decoder = NULL;
		IWICFormatConverter* converter = NULL;

		// Create a WIC stream to map onto the memory.
		auto hr = WicFactory::Get()->CreateStream(&stream);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		// Initialize the stream with the memory pointer and size.
		hr = stream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(file_address),
			static_cast<DWORD>(file_size)
		);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		hr = WicFactory::Get()->CreateDecoderFromStream(
			stream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
		if (FAILED(hr)) { ExceptionDialog(load_file_exception); goto Exception; }

		converter = LoadFromDecoder(decoder);
		if (converter == nullptr) { goto Exception; }

	Exception:
		SafeRelease(&decoder);
		SafeRelease(&stream);
		return converter;
	}
};


ImageResource::ImageResource(const wstring& file_name) :
	_wic_image(WicFactory::Load(file_name)), _wic_bitmap(nullptr),
	_bitmap_data(nullptr), _d2d_bitmap(nullptr) {
}

ImageResource::ImageResource(void* file_address, size_t file_size) :
	_wic_image(WicFactory::Load(file_address, file_size)), _wic_bitmap(nullptr),
	_bitmap_data(nullptr), _d2d_bitmap(nullptr) {
}


ImageResource::~ImageResource() {
	SafeRelease(&_wic_image);
	SafeRelease(&_wic_bitmap);
	SafeRelease(&_d2d_bitmap);
}

Size ImageResource::GetSize() {
	if (IsEmpty()) { return size_min; }
	Size size;
	_wic_image->GetSize(&size.width, &size.height);
	return size;
}

Color ImageResource::GetColor(Point point) {
	if (IsEmpty()) { ExceptionDialog(); return ColorSet::White; }

	Size size = GetSize();
	uint x = static_cast<uint>(point.x), y = static_cast<uint>(point.x);
	if (x >= size.width || y >= size.height) {
		ExceptionDialog(); return ColorSet::White;
	}

	if (_wic_bitmap == nullptr) {
		auto hr = WicFactory::Get()->CreateBitmapFromSource(_wic_image, WICBitmapNoCache, &_wic_bitmap);
		if (FAILED(hr)) { ExceptionDialog(); return ColorSet::White; }
	}

	if (_bitmap_data == nullptr) {
		WICRect lock_region = { 0, 0, size.width, size.height };
		auto hr = _wic_bitmap->Lock(&lock_region, WICBitmapLockWrite, &_bitmap_data);
		if (FAILED(hr)) { ExceptionDialog(); return ColorSet::White; }
	}

	const int byte_per_pixel = 4;
	WICPixelFormatGUID format;
	UINT stride = 0;
	UINT buffer_size = 0;
	BYTE* data = NULL;
	uint offset = 0;

	auto hr = _bitmap_data->GetPixelFormat(&format);
	if (FAILED(hr)) { ExceptionDialog(); goto Exception; }
	if (format != GUID_WICPixelFormat32bppPBGRA) { ExceptionDialog(); goto Exception; }

	hr = _bitmap_data->GetStride(&stride);
	if (FAILED(hr)) { ExceptionDialog(); goto Exception; }

	hr = _bitmap_data->GetDataPointer(&buffer_size, &data);
	if (FAILED(hr)) { ExceptionDialog(); goto Exception; }

	offset = y * (size.width * byte_per_pixel + stride) + x * byte_per_pixel;
	if (offset + byte_per_pixel > buffer_size) { ExceptionDialog(); goto Exception; }
	
	return *reinterpret_cast<Color*>(data + offset);

Exception:
	SafeRelease(&_bitmap_data);
	return ColorSet::White;
}


END_NAMESPACE(WndDesign)