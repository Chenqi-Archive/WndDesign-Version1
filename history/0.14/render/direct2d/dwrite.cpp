#include "dwrite.h"
#include "../../common/exception.h"

#undef Alloc
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "dwrite.lib")


BEGIN_NAMESPACE(WndDesign)


inline D2D1_SIZE_F SizeToSIZE(Size size) {
	return D2D1::SizeF(static_cast<FLOAT>(size.width), static_cast<FLOAT>(size.height));
}

inline Size SIZEToSize(D2D1_SIZE_F size) {
	return { static_cast<uint>(size.width), static_cast<uint>(size.height) };
}


extern HRESULT hr;
inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { std::abort(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)


class DWriteFactory {
private:
	class Factory {
	public:
		IDWriteFactory* factory;
		Factory():factory(NULL) { 
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, 
									 __uuidof(IDWriteFactory), 
									 reinterpret_cast<IUnknown**>(&factory)); 
			AbortWhenFailed(hr); 
		}
		~Factory() { 
			factory->Release(); 
		}
	};
public:
	static Ref IDWriteFactory* Get() {
		static Factory factory;
		return factory.factory;
	}
};


void TextLayout::Destroy() {
	layout->Release();
	format->Release();
}

TextLayout CreateTextLayout(const wstring& text, Size size, const TextBoxStyle& style) {
	IDWriteTextFormat* format;
	IDWriteTextLayout* layout;
	hr = DWriteFactory::Get()->CreateTextFormat(style.font._family,
												NULL,
												static_cast<DWRITE_FONT_WEIGHT>(style.font._weight),
												static_cast<DWRITE_FONT_STYLE>(style.font._style),
												static_cast<DWRITE_FONT_STRETCH>(style.font._stretch),
												static_cast<FLOAT>(style.font._size),
												style.font._locale,
												&format);
	AbortWhenFailed(hr);
	hr = DWriteFactory::Get()->CreateTextLayout(text.c_str(), 
												static_cast<UINT32>(text.length()), 
												format, 
												static_cast<FLOAT>(size.width), 
												static_cast<FLOAT>(size.height),
												&layout);
	layout->SetTextAlignment(static_cast<DWRITE_TEXT_ALIGNMENT>(style.paragraph._text_align));
	layout->SetParagraphAlignment(static_cast<DWRITE_PARAGRAPH_ALIGNMENT>(style.paragraph._paragraph_align));
	layout->SetFlowDirection(static_cast<DWRITE_FLOW_DIRECTION>(style.paragraph._flow_direction));
	layout->SetReadingDirection(static_cast<DWRITE_READING_DIRECTION>(style.paragraph._read_direction));
	layout->SetWordWrapping(static_cast<DWRITE_WORD_WRAPPING>(style.paragraph._word_wrap));
	
	return { format, layout };
}

TextLayout CreateTextLayout(const wstring& text, Size size, const TextBoxStyleEx& style) {

	//

	return { nullptr, nullptr };
}

END_NAMESPACE(WndDesign)

