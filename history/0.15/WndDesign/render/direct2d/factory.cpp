#include "factory.h"

#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")


BEGIN_NAMESPACE(WndDesign)

inline D2D1_RECT_F Rect2RECT(Rect rect) {
	return D2D1::RectF(static_cast<FLOAT>(rect.point.x), static_cast<FLOAT>(rect.point.y),
					   static_cast<FLOAT>(rect.RightBottom().x), static_cast<FLOAT>(rect.RightBottom().y));
}


template<class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


HRESULT hr;
inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { std::abort(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)


class Factory {
private:
	friend ID2D1Factory* GetFactroy();
	ID2D1Factory* factory;
	Factory() :factory(NULL) {
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
		AbortWhenFailed(hr);
	}
	~Factory() {
		SafeRelease(&factory);
	}
};

Ref<ID2D1Factory*> GetFactroy() {
	static Factory factory;
	return factory.factory;
}


ID2D1RoundedRectangleGeometry* CreateRoundedRectangleGeometry(Rect region, uint radius) {
	ID2D1RoundedRectangleGeometry* geometry;
	GetFactroy()->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(Rect2RECT(region), static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
		&geometry);
	return geometry;
}


END_NAMESPACE(WndDesign)