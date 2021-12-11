#include "tile.h"

#include <random>
#include <ctime>

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

void Init() {
	// Direct2D
	res = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);
	if (!SUCCEEDED(res)) { _asm int 3 }

	srand(time(nullptr));
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
		tile_cache.Init(hwnd_target, 
						region_infinite, 
						{ point_zero, { static_cast<uint>(rc.right - rc.left), static_cast<uint>(rc.bottom - rc.top) } },
						{ 256, 256 });
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

void OnSize(uint w, uint h) {
	if (hwnd_target == NULL) {
		return;
	}
	hwnd_target->Resize(D2D1::SizeU(w, h));
	if (!SUCCEEDED(res)) { _asm int 3 }

	tile_cache.SetCurrentRegion({ tile_cache.GetCurrentRegion().point, { w, h } });
}

void OnUpdate() {
	uint color = (rand() << 16) + rand();
	Size size = { 1000, 1000 };
	int x, y;
	uint w, h;
	x = rand() % size.width - 500;
	y = rand() % size.height - 500;
	w = rand() % size.width;
	h = rand() % size.height;
	Rect region = tile_cache.GetCurrentRegion();
	region = region + Vector{ x, y };
	region.size = { w, h };

	tile_cache.DrawColor(color, region);
	OnPaint();
}

void OnDrag(int dx, int dy) {
	Vector vec = { dx, dy };
	tile_cache.SetCurrentRegion(tile_cache.GetCurrentRegion() - vec);
	OnPaint();
}