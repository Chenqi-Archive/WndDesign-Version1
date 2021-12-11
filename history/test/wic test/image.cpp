
#include <string>

#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

using std::wstring;


extern void breakpoint();
#define BreakWhenFailed(hr) if(FAILED(hr)) {breakpoint();}

IWICImagingFactory* pIWICFactory = NULL;
IWICBitmapSource* pBitmap = NULL;

template<class Interface>
inline void SafeRelease(Interface * *ppInterfaceToRelease) {
    if (*ppInterfaceToRelease != NULL) {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

void WicInit(const wstring& uri) {
    // Initialize COM
    CoInitialize(NULL);

    // Create the COM imaging factory
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pIWICFactory)
    );
    BreakWhenFailed(hr);

    IWICBitmapDecoder* pDecoder = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename(
        uri.c_str(),
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &pDecoder
    );
    BreakWhenFailed(hr);

    IWICBitmapFrameDecode* pSource = NULL;
    hr = pDecoder->GetFrame(0, &pSource);
    BreakWhenFailed(hr);

    IWICFormatConverter* pConverter = NULL;
    hr = pIWICFactory->CreateFormatConverter(&pConverter);
    BreakWhenFailed(hr);
    hr = pConverter->Initialize(
        pSource,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
    );
    BreakWhenFailed(hr);

    pBitmap = pConverter;

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);

    if (pBitmap == nullptr) {
        breakpoint();
    }
}