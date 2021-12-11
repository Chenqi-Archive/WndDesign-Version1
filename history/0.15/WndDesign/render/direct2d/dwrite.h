#pragma once

#include "../../common/core.h"
#include "../../common/feature.h"

#include "../../style/textbox_style.h"
#include "../../style/textbox_style_extended.h"


/////////////////////////////////////////////////////////
////          DirectWrite Resources Manager          ////
/////////////////////////////////////////////////////////

//// DirectWrite Types

struct IDWriteTextLayout;
struct IDWriteTextFormat;


BEGIN_NAMESPACE(WndDesign)

class TextLayout : Uncopyable{
private:
	friend class Tile;
	Alloc<IDWriteTextFormat*> _format;
	Alloc<IDWriteTextLayout*> _layout;

private:
	const wstring& _text;
	const TextBoxStyle& _style;
	Size _max_size;

public:
	TextLayout(const wstring& text, const TextBoxStyle& style);
	~TextLayout();
	void TextChanged();
	void SetMaxSize(Size max_size);
};


END_NAMESPACE(WndDesign)
