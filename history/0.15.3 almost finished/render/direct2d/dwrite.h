#pragma once

#include "../../common/core.h"
#include "../../common/feature.h"

#include "../../style/textbox_style.h"

#include <string>


/////////////////////////////////////////////////////////
////          DirectWrite Resources Manager          ////
/////////////////////////////////////////////////////////

//// DirectWrite Types

struct IDWriteTextLayout;
struct IDWriteTextFormat;


BEGIN_NAMESPACE(WndDesign)

using std::wstring;


class TextLayout : Uncopyable{
private:
	friend class Tile;
	Alloc<IDWriteTextFormat*> _format;
	Alloc<IDWriteTextLayout*> _layout;

private:
	const wstring& _text;        // Reference to a string that may change.
	const TextBoxStyle& _style;	 // Reference to the textbox style.
	Size _max_size;				 // The max size of the textbox.

public:
	TextLayout(const wstring& text, const TextBoxStyle& style);
	~TextLayout();

	void TextChanged();  // Update the layout attributes when text has changed.
	void SetMaxSize(Size max_size);

	// Automatially calculate width and height. max_size will also be reset.
	// Returns the size of the bounding region of text.
	Size AutoFitSize(uint width_max); 
};


END_NAMESPACE(WndDesign)
