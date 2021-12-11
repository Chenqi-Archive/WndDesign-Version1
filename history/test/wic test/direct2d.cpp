﻿#include <string>
#include <vector>

using namespace std;

#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace {  // static

ID2D1Factory* factory;
ID2D1HwndRenderTarget* target;

ID2D1SolidColorBrush* brush;

ID2D1Bitmap* bitmap;


IDWriteFactory* writeFactory;
IDWriteTextFormat* textFormat;
IDWriteTextLayout* textLayout;
IDWriteFontCollection* fontCollection;

//IDWriteTextRenderer* textRenderer;

HRESULT res = S_OK;

}

extern HWND hWnd;

//wstring text = L"The application can then render the text \
//using the DrawTextLayout function provided by Direct2D or by \
//implementing a callback function that can use GDI, Direct2D, \
//or other graphics 😁 systems to render the glyphs. \n For a single \
//format text, the DrawText function on Direct2D provides a simpler \
//way to draw text without having to first create a IDWriteTextLayout object."; /**/

//wstring text = L"The😁application can then render the text using the\nDrawTextLayout \nfunction provided"; /**/

wstring text = L"a😳b\r\nab\nab";

wstring file = L"E:\\Cognitive Architecture\\个人素材\\icon.jpg";

//wstring uri = L"https://apod.nasa.gov/apod/image/1711/BigDipperMt2_Cullen_960.jpg";

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


extern void WicInit(const wstring& uri);
extern IWICBitmapSource* pBitmap;

void breakpoint() {
	//__asm int 3
}
#define BreakWhenFailed(hr) if(FAILED(hr)) {breakpoint();}

void Init() {
	// Wic
	WicInit(file);

	// Direct2D
	res = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&factory
	);
	BreakWhenFailed(res);

	// DirectWrite
	res = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&writeFactory)
	);
	BreakWhenFailed(res);

	//res = writeFactory->GetSystemFontCollection(&fontCollection, TRUE);
	//if (!SUCCEEDED(res)) { _asm int 3 }

	res = writeFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"en-US",
		&textFormat
	);
	BreakWhenFailed(res);

	res = writeFactory->CreateTextLayout(
		text.c_str(),
		text.length(),
		textFormat,
		500.0f, 200.0f,
		&textLayout
	);
	BreakWhenFailed(res);

	//// Set text format
	//DWRITE_TEXT_RANGE range = { 4, 11 }; // "application"
	//res = textLayout->SetFontSize(50.0f, range);
	//BreakWhenFailed(res);

	//range = { 16, 3 };
	//res = textLayout->SetUnderline(true, range);
	//BreakWhenFailed(res);

	//range = { 20, 4 };
	//res = textLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, range);
	//BreakWhenFailed(res);

	//res = textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	//BreakWhenFailed(res);


	FLOAT x, y;
	DWRITE_HIT_TEST_METRICS metrics;
	res = textLayout->HitTestTextPosition(2, true, &x, &y, &metrics);
	BreakWhenFailed(res);

	//FLOAT width;
	//textLayout->DetermineMinWidth(&width);

	//width = textLayout->GetMaxWidth();

	//DWRITE_OVERHANG_METRICS overhangs;
	//textLayout->GetOverhangMetrics(&overhangs);

	//DWRITE_TEXT_METRICS textmetrics;
	//textLayout->GetMetrics(&textmetrics);
}

void Final() {
	SafeRelease(&textFormat);
	SafeRelease(&textLayout);
	SafeRelease(&writeFactory);

	SafeRelease(&factory);
}


enum class CaretState { None, Static, BlinkShow, BlinkHide } caret_state;
DWRITE_HIT_TEST_METRICS caret_metrics;

enum class SelectState {None, Selected} select_state;
vector<DWRITE_HIT_TEST_METRICS> select_metrics;
UINT32 startpos = 0;
UINT32 endpos = 0;


void OnPaint() {
	//static bool should_paint = false;
	//if (should_paint == false) {
	//	should_paint = true;
	//} else {
	//	return;
	//}

	if (target == NULL) {
		RECT rc;
		GetClientRect(hWnd, &rc);

		res = factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
			&target
		);
		BreakWhenFailed(res);
	}

	if (brush == NULL) {
		res = target->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&brush
		);
		BreakWhenFailed(res);
	}

	if (bitmap == NULL) {
		if (pBitmap == nullptr) { breakpoint(); }

		res = target->CreateBitmapFromWicBitmap(
			pBitmap,
			NULL, 
			&bitmap
		);
		BreakWhenFailed(res);
	}

	target->BeginDraw();

	target->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.1F));

	//target->DrawText(
	//	text.c_str(),
	//	text.length(),
	//	textFormat,
	//	D2D1::RectF(100.f, 100.f, 500.f, 300.f),
	//	brush
	//);

	target->DrawBitmap(bitmap, D2D1::RectF(100.f, 100.f, 600.f, 300.f));

	brush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));

	target->DrawRectangle(D2D1::RectF(100.0f, 100.0f, 600.0f, 300.0f), brush);


	//// Test if one can clear a region of target
	//target->PushAxisAlignedClip(D2D1::RectF(100.0f, 100.0f, 600.0f, 300.0f), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	//target->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
	//target->PopAxisAlignedClip();

	target->DrawTextLayout(
		D2D1::Point2F(100.0f, 100.0f),
		textLayout,
		brush,
		D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT   // For color fonts like emoji :)
	);

	// Draw selection
	brush->SetColor(D2D1::ColorF(D2D1::ColorF::DimGray, 0.5));
	if (select_state == SelectState::Selected) {
		for (auto it : select_metrics) {
			target->FillRectangle(D2D1::RectF(
				it.left,
				it.top,
				it.left + it.width,
				it.top + it.height), brush);
		}
	}

	// Draw caret if not selected.
	else if (caret_state == CaretState::Static || caret_state == CaretState::BlinkShow) {
		target->FillRectangle(D2D1::RectF(
			caret_metrics.left + 100,
			caret_metrics.top + 100,
			caret_metrics.left + 100 + 1,
			caret_metrics.top + 100 + caret_metrics.height), brush);
	}


	res = target->EndDraw();

	if (!SUCCEEDED(res)) {
		// Destroy the device-dependent resources.
		SafeRelease(&target);
		SafeRelease(&brush);
		SafeRelease(&bitmap);
	}
}

void OnSize(int w, int h) {
	if (target == NULL) {
		return;
	}
	target->Resize(D2D1::SizeU(w, h));
}

void OnHover(int x, int y) {
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS hitTestMetrics;

	textLayout->HitTestPoint(x - 100, y - 100, &isTrailingHit, &isInside, &hitTestMetrics);

	if (isInside == true) {
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
		return;
	}

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void OnLeftDown(int x, int y) {
	static bool a = false;
	static std::vector<ID2D1BitmapRenderTarget*> targets;
	if (a) {
		targets.resize(1000);
		for (auto& it : targets) {
			res = target->CreateCompatibleRenderTarget(D2D1_SIZE_F{1000, 1000}, &it);
			it->Clear(D2D1::ColorF(D2D1::ColorF::Red));
			it->DrawRectangle(D2D1::RectF(100.0f, 100.0f, 600.0f, 300.0f), brush);
		}
		a = false;
	} else {
		for (auto& it : targets) {
			it->Release();
		}
		targets.clear();
		a = true;
	}


	BOOL isTrailingHit;
	BOOL isInside;

	textLayout->HitTestPoint(x - 100, y - 100, &isTrailingHit, &isInside, &caret_metrics);

	if (isTrailingHit) {
		caret_metrics.left += caret_metrics.width;
	}
	// Check the hitTestMetrics::length in case for 😁.

	//FLOAT xf, yf;
	//textLayout->HitTestTextPosition(hitTestMetrics.textPosition + hitTestMetrics.length - 1, isTrailingHit, &xf, &yf, &caret_metrics);

	caret_state = CaretState::BlinkShow;
	select_state = SelectState::None;

	OnPaint();
}

// Select a range of text, with x1,y1 as the starting point(when leftdown), 
//   and x2,y2 at the end point(when moving or leftup).
void OnSelect(int x1, int y1, int x2, int y2) {
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS hitTestMetrics;

	textLayout->HitTestPoint(x1 - 100, y1 - 100, &isTrailingHit, &isInside, &hitTestMetrics);
	startpos = hitTestMetrics.textPosition + (isTrailingHit ? hitTestMetrics.length : 0);
	
	// The last character should also be included.
	// Check the hitTestMetrics::length in case for 😁.

	textLayout->HitTestPoint(x2 - 100, y2 - 100, &isTrailingHit, &isInside, &hitTestMetrics);
	endpos = hitTestMetrics.textPosition + (isTrailingHit ? hitTestMetrics.length : 0);

	if (startpos == endpos) { return; }

	if (startpos > endpos) {
		std::swap(startpos, endpos);
	}

	// Now the selected text range is [startpos, endpos).

	// May also support double-click to select a whole word.


	UINT32 line_cnt;
	textLayout->GetLineMetrics(nullptr, 0, &line_cnt);

	DWRITE_TEXT_METRICS textMetrics;
	textLayout->GetMetrics(&textMetrics);

	UINT32 actual_size = line_cnt * textMetrics.maxBidiReorderingDepth; // The assumed actual line size.
	do{
		select_metrics.resize(actual_size);
		textLayout->HitTestTextRange(startpos, endpos - startpos, 100, 100, 
									 select_metrics.data(), select_metrics.size(), &actual_size);
	} while (actual_size > select_metrics.size());

	select_metrics.resize(actual_size);
	if (actual_size == 1 && select_metrics.front().length == 0) {
		// No character is actually selected. 
		// This may occur when hit inside a 2-wchar_t length unicode character like 😁.
		// Update: Now it has been fixed elsewhere. Control will never reach here.
		return;
	}

	select_state = SelectState::Selected;

	OnPaint();
}