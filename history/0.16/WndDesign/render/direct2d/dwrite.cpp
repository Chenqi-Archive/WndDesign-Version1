#include "dwrite.h"
#include "../../common/exception.h"

#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "dwrite.lib")


BEGIN_NAMESPACE(WndDesign)


inline D2D1_SIZE_F SizeToSIZE(Size size) {
	return D2D1::SizeF(static_cast<FLOAT>(size.width), static_cast<FLOAT>(size.height));
}

inline Size SIZEToSize(D2D1_SIZE_F size) {
	return Size(static_cast<uint>(size.width), static_cast<uint>(size.height));
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


TextLayout::TextLayout(const wstring& text, const TextBoxStyle& style) :
	_format(nullptr), _layout(nullptr), _text(text), _style(style), _max_size(size_min) {
	auto hr = DWriteFactory::Get()->CreateTextFormat(_style.font._family,
													 NULL,
													 static_cast<DWRITE_FONT_WEIGHT>(_style.font._weight),
													 static_cast<DWRITE_FONT_STYLE>(_style.font._style),
													 static_cast<DWRITE_FONT_STRETCH>(_style.font._stretch),
													 static_cast<FLOAT>(_style.font._size),
													 _style.font._locale,
													 &_format);
	AbortWhenFailed(hr);
	TextChanged();
}

TextLayout::~TextLayout() {
	SafeRelease(&_format);
	SafeRelease(&_layout);
}

void TextLayout::TextChanged() {
	SafeRelease(&_layout);
	auto hr = DWriteFactory::Get()->CreateTextLayout(_text.c_str(),
													 static_cast<UINT32>(_text.length()),
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

	// Set the text range styles.

}

void TextLayout::SetMaxSize(Size max_size) {
	if (_max_size == max_size) { return; }
	_max_size = max_size;
	_layout->SetMaxWidth(static_cast<FLOAT>(_max_size.width));
	_layout->SetMaxHeight(static_cast<FLOAT>(_max_size.height));
}

const Size TextLayout::AutoFitSize(uint width_max) const {
	_layout->SetMaxWidth(static_cast<FLOAT>(width_max));

	DWRITE_TEXT_METRICS metrics;
	_layout->GetMetrics(&metrics);

	return Size(
		static_cast<uint>(metrics.widthIncludingTrailingWhitespace), 
		static_cast<uint>(metrics.height) 
	);
}


END_NAMESPACE(WndDesign)
