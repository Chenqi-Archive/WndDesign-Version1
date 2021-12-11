#pragma once

#include "../../common/core.h"

#include "../../style/textbox_style.h"
#include "../../style/textbox_style_extended.h"


/////////////////////////////////////////////////////////
////          DirectWrite Resources Manager          ////
/////////////////////////////////////////////////////////

//// DirectWrite Types

struct IDWriteTextLayout;
struct IDWriteTextFormat;


BEGIN_NAMESPACE(WndDesign)

struct TextLayout {
private:
	Alloc IDWriteTextFormat* format;
	Alloc IDWriteTextLayout* layout;
public:
	TextLayout(Alloc IDWriteTextFormat* format, Alloc IDWriteTextLayout* layout) :
		format(format), layout(layout) {}
	Ref IDWriteTextLayout* Get() const { return layout; }
	void Destroy();
};

TextLayout CreateTextLayout(const wstring& text, Size size, const TextBoxStyle& style);
TextLayout CreateTextLayout(const wstring& text, Size size, const TextBoxStyleEx& style);

END_NAMESPACE(WndDesign)
