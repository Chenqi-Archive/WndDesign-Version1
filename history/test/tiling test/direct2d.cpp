#include "tile.h"

#include <random>

#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace {  // static

ID2D1Factory* factory;

TileCache tile_cache;

ID2D1HwndRenderTarget* hwnd_target;

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

	if (tile_cache.IsEmpty()) {
		RECT rc;
		GetClientRect(hWnd, &rc);
		tile_cache.Init(hwnd_target, { rc.right - rc.left, rc.bottom - rc.top }, { 256, 256 });
	}
}

void DestroyDeviceDependentResources() {
	SafeRelease(&hwnd_target);
	tile_cache.Clear();
}

void Final() {
	tile_cache.Clear();
	SafeRelease(&factory);
}


void OnPaint() {
	CreateDeviceDependentResources();

	tile_cache.DrawOn(hwnd_target);

	if (!SUCCEEDED(res)) {
		DestroyDeviceDependentResources();
	}
}

void OnSize(int w, int h) {
	if (hwnd_target == NULL) {
		return;
	}
	hwnd_target->Resize(D2D1::SizeU(w, h));
	if (!SUCCEEDED(res)) { _asm int 3 }

	tile_cache.SetSize({ w, h });
}

void OnUpdate() {
	uint color = rand() << 16 + rand();
	int x, y;
	uint w, h;
	x = rand() % tile_cache.GetSize().width;
	y = rand() % tile_cache.GetSize().height;
	w = rand() % tile_cache.GetSize().width;
	h = rand() % tile_cache.GetSize().height;

	tile_cache.DrawColor(color, { {x, y}, {w, h} });
	tile_cache.DrawOn(hwnd_target);
}
