#include "dwrite.h"
#include "../../common/exception.h"

// For rounding the float value to int.
#include <cmath>

#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "dwrite.lib")


BEGIN_NAMESPACE(WndDesign)


inline D2D1_SIZE_F Size2SIZE(Size size) {
	return D2D1::SizeF(static_cast<FLOAT>(size.width), static_cast<FLOAT>(size.height));
}

inline Size SIZE2Size(D2D1_SIZE_F size) {
	return Size(static_cast<uint>(size.width), static_cast<uint>(size.height));
}

inline D2D1_POINT_2F Point2POINT(Point point) {
	return D2D1::Point2F(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
}

inline Point POINT2Point(D2D1_POINT_2F point) {
	return Point(static_cast<uint>(point.x), static_cast<uint>(point.y));
}


template<class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { Error(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)


class DWriteFactory {
private:
	IDWriteFactory* factory;
	DWriteFactory() :factory(NULL) {
		auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
									  __uuidof(IDWriteFactory),
									  reinterpret_cast<IUnknown**>(&factory));
		AbortWhenFailed(hr);
	}
	~DWriteFactory() {
		SafeRelease(&factory);
	}
public:
	static Ref<IDWriteFactory*> Get() {
		static DWriteFactory factory;
		return factory.factory;
	}
};


const wstring TextLayout::_empty_text = L"";

TextLayout::TextLayout(const TextBoxStyle& style) :
	_format(nullptr), _layout(nullptr), _text(&_empty_text), _style(style), _max_size(size_min) {
	auto hr = DWriteFactory::Get()->CreateTextFormat(_style.font._family,
													 NULL,
													 static_cast<DWRITE_FONT_WEIGHT>(_style.font._weight),
													 static_cast<DWRITE_FONT_STYLE>(_style.font._style),
													 static_cast<DWRITE_FONT_STRETCH>(_style.font._stretch),
													 static_cast<FLOAT>(_style.font._size),
													 _style.font._locale,
													 &_format);
	AbortWhenFailed(hr);
	//TextUpdated();  // The text must be empty at construction.
}

TextLayout::~TextLayout() {
	SafeRelease(&_format);
	SafeRelease(&_layout);
}

void TextLayout::SetText(Ref<const wstring*> text) {
	_text = text;
	if (_text == nullptr) {
		_text = &_empty_text;
	}
	TextUpdated();
}

void TextLayout::TextUpdated() {
	SafeRelease(&_layout);
	auto hr = DWriteFactory::Get()->CreateTextLayout(_text->c_str(),
													 static_cast<UINT32>(_text->length()),
													 _format,
													 static_cast<FLOAT>(_max_size.width),
													 static_cast<FLOAT>(_max_size.height),
													 &_layout);
	AbortWhenFailed(hr);
	_layout->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(_style.paragraph._text_align));
	_layout->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(_style.paragraph._paragraph_align));
	_layout->SetFlowDirection(static_cast<DWRITE_FLOW_DIRECTION>(_style.paragraph._flow_direction));
	_layout->SetReadingDirection(static_cast<DWRITE_READING_DIRECTION>(_style.paragraph._read_direction));
	_layout->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(_style.paragraph._word_wrap));
	ValueTag line_height = _style.paragraph._line_height;
	if (line_height.IsPixel()) {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, static_cast<FLOAT>(line_height.AsUnsigned()), 0.7F * line_height.AsUnsigned());
	} else if (line_height.IsPercent()) {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, line_height.AsUnsigned() / 100.0F, 1.1F);
	} else {
		_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 0.0F, 0.0F);  // The last two parameters are ignored.
	}
	if (_style.paragraph._tab_size != 0) {
		_layout->SetIncrementalTabStop(static_cast<FLOAT>(_style.paragraph._tab_size));
	}
	// Set the text range styles.

}

void TextLayout::SetMaxSize(Size max_size) {
	if (_max_size.width != max_size.width) {
		_max_size.width = max_size.width; _layout->SetMaxWidth(static_cast<FLOAT>(_max_size.width));
	}
	if (_max_size.height != max_size.height) {
		_max_size.height = max_size.height; _layout->SetMaxHeight(static_cast<FLOAT>(_max_size.height));
	}
}

const Size TextLayout::AutoFitSize(Size max_size) {
	SetMaxSize(max_size);

	DWRITE_TEXT_METRICS metrics;
	_layout->GetMetrics(&metrics);

#pragma message ("The order might be different for vertical flow direction.")

	return Size(
		static_cast<uint>(ceil(metrics.widthIncludingTrailingWhitespace)),  // Round up the size.
		static_cast<uint>(ceil(metrics.height))
	);
}

const HitTestInfo TextLayout::HitTestPoint(Point point) const {
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS metrics;
	_layout->HitTestPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y), &isTrailingHit, &isInside, &metrics);
	return HitTestInfo{
		metrics.textPosition,
		metrics.length,
		Rect(POINT2Point(D2D1::Point2F(metrics.left, metrics.top)), SIZE2Size(D2D1::SizeF(metrics.width, metrics.height))),
		static_cast<bool>(isInside),
		static_cast<bool>(isTrailingHit),
	};
}

const HitTestInfo TextLayout::HitTestTextPosition(uint text_position) const {
	FLOAT x, y;
	DWRITE_HIT_TEST_METRICS metrics;
	_layout->HitTestTextPosition(text_position, false, &x, &y, &metrics);
	return HitTestInfo{
		metrics.textPosition,
		metrics.length,
		Rect(POINT2Point(D2D1::Point2F(metrics.left, metrics.top)), SIZE2Size(D2D1::SizeF(metrics.width, metrics.height))),
		true,
		false,
	};
}

void TextLayout::HitTestTextRange(uint text_position, uint text_length, vector<HitTestInfo>& geometry_regions) const {
	vector<DWRITE_HIT_TEST_METRICS> metrics;

	UINT32 line_cnt;
	_layout->GetLineMetrics(nullptr, 0, &line_cnt);

	UINT32 actual_size = line_cnt; // The assumed actual line size.
	do {
		metrics.resize(actual_size);
		_layout->HitTestTextRange(text_position, text_length, 0, 0, metrics.data(), static_cast<UINT32>(metrics.size()), &actual_size);
	} while (actual_size > metrics.size());
	metrics.resize(actual_size);

	geometry_regions.resize(metrics.size());
	for (size_t i = 0; i < geometry_regions.size(); ++i) {
		geometry_regions[i] = HitTestInfo{
			metrics[i].textPosition,
			metrics[i].length,
			Rect(POINT2Point(D2D1::Point2F(metrics[i].left, metrics[i].top)), SIZE2Size(D2D1::SizeF(metrics[i].width, metrics[i].height))),
			true,
			false,
		};
	}
}


END_NAMESPACE(WndDesign)
