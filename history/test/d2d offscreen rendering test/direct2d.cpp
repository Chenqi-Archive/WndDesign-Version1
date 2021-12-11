#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace {  // static

ID2D1Factory* factory;

ID2D1HwndRenderTarget* hwnd_target;
ID2D1BitmapRenderTarget* bitmap_target;



HRESULT res = S_OK;

}

extern HWND hWnd;

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

void Init() {
	// Direct2D
	res = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);
	if (!SUCCEEDED(res)) { _asm int 3 }

}

void Final() {
	SafeRelease(&factory);
}

void CreateDeviceDependentResources() {
	if (hwnd_target == NULL) {
		RECT rc;
		GetClientRect(hWnd, &rc);

		res = factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
			&hwnd_target
		);
		if (!SUCCEEDED(res)) { _asm int 3 }
	}



	if (bitmap_target == NULL) {
		res = hwnd_target->CreateCompatibleRenderTarget(&bitmap_target);
		if (!SUCCEEDED(res)) { _asm int 3 }
	}
}

void OnPaint() {
	CreateDeviceDependentResources();

	hwnd_target->BeginDraw();

	ID2D1Bitmap* bitmap;
	bitmap_target->GetBitmap(&bitmap);
	hwnd_target->DrawBitmap(bitmap,
							D2D1::RectF(0, 0, hwnd_target->GetSize().width, hwnd_target->GetSize().height),
							1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
							D2D1::RectF(0, 0, hwnd_target->GetSize().width, hwnd_target->GetSize().height));

	res = hwnd_target->EndDraw();

	if (!SUCCEEDED(res)) {
		// Destroy the device-dependent resources.
		SafeRelease(&hwnd_target);
	}
}

void OnSize(int w, int h) {
	if (hwnd_target == NULL || bitmap_target == NULL) {
		return;
	}
	hwnd_target->Resize(D2D1::SizeU(w, h));
	SafeRelease(&bitmap_target);
	res = hwnd_target->CreateCompatibleRenderTarget(&bitmap_target);
	if (!SUCCEEDED(res)) { _asm int 3 }
}

void OnUpdate() {
	static UINT32 color = 0xFFFFFFFF;

	color += 0xF0;

	bitmap_target->BeginDraw();

	bitmap_target->Clear(D2D1::ColorF(color));

	res = bitmap_target->EndDraw();
	if (!SUCCEEDED(res)) {
	// Destroy the device-dependent resources.
		SafeRelease(&bitmap_target);
	}

	OnPaint();
}
