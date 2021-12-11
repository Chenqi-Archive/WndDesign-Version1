#pragma once

#include "../../common/core.h"
#include "../../common/feature.h"

#include "../../style/textbox_style.h"

#include <string>
#include <vector>


/////////////////////////////////////////////////////////
////          DirectWrite Resources Manager          ////
/////////////////////////////////////////////////////////

//// DirectWrite Types

struct IDWriteTextLayout;
struct IDWriteTextFormat;


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::vector;


class TextLayout : Uncopyable{
private:
	friend class Tile;
	Alloc<IDWriteTextFormat*> _format;
	Alloc<IDWriteTextLayout*> _layout;

private:
	static const wstring _empty_text;  // L""

	Ref<const wstring*> _text;   // Reference to a string that may change.
	const TextBoxStyle& _style;	 // Reference to the textbox style.
	Size _max_size;				 // The max size of the textbox.

public:
	TextLayout(const TextBoxStyle& style);
	~TextLayout();

	void SetText(Ref<const wstring*> text);
	void TextUpdated();  // Update the layout attributes when text has changed.
	void SetMaxSize(Size max_size);

	// Automatially calculate width and height depending on max_size. max_size will also be reset.
	const Size AutoFitSize(Size max_size);

	// Wrapper functions for hit testing.
	const HitTestInfo HitTestPoint(Point point) const; // Point is relative to text layout.
	const HitTestInfo HitTestTextPosition(uint text_position) const;
	void HitTestTextRange(uint text_position, uint text_length, vector<HitTestInfo>& geometry_regions) const;
};


END_NAMESPACE(WndDesign)
