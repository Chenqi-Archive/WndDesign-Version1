#pragma once

#include "wnd_style.h"


BEGIN_NAMESPACE(WndDesign)


enum class TextAlign : uchar {
	Leading,
	Trailing,
	Center,
	Justified,
};

enum class ParagraphAlign : uchar {
	Top,
	Bottom,
	Center,
};

enum class FlowDirection : uchar {
	TopToBottom = 0,
	BottomToTop = 1,
	LeftToRight = 2,
	RightToLeft = 3,
};

enum class ReadDirection : uchar {
	LeftToRight = 0,
	RightToLeft = 1,
	TopToBottom = 2,
	BottomToTop = 3,
};

enum class WordWrap : uchar {
	Wrap = 0,
	NoWrap = 1,
	EmergencyBreak = 2,
	WholeWord = 3,
	Character = 4,
};

enum class FontWeight : ushort {
	Thin = 100,
	ExtraLight = 200,
	UltraLight = 200,
	Light = 300,
	SemiLight = 350,
	Normal = 400,
	Regular = 400,
	Medium = 500,
	DemiBold = 600,
	SemiBold = 600,
	Bold = 700,
	ExtraBold = 800,
	UltraBold = 800,
	Black = 900,
	Heavy = 900,
	ExtraBlack = 950,
	UltraBlack = 950,
};

enum class FontStretch : uchar {
	Undefined = 0,
	Ultra_condensed = 1,
	Extra_condensed = 2,
	Condensed = 3,
	Semi_condensed = 4,
	Normal = 5,
	Medium = 5,
	Semi_expanded = 6,
	Expanded = 7,
	Extra_expanded = 8,
	Ultra_expanded = 9
};

enum class FontStyle : uchar {
	Normal,
	Oblique,
	Italic
};


// For static text boxes with a single font format.
struct TextBoxStyle : WndStyle {
public:
	// Padding between text content and the inner side of the border.
	struct PaddingStyle {
	public:
		LengthTag _left = length_auto;
		LengthTag _top = length_auto;
		LengthTag _right = length_auto;
		LengthTag _down = length_auto;
	public:
		constexpr PaddingStyle& left(LengthTag left) { _left = left; return *this; }
		constexpr PaddingStyle& top(LengthTag top) { _top = top; return *this; }
		constexpr PaddingStyle& right(LengthTag right) { _right = right; return *this; }
		constexpr PaddingStyle& down(LengthTag down) { _down = down; return *this; }
		constexpr void SetAll(LengthTag all) { _left = _top = _right = _down = all; }
	}padding;

	// Paragraph styles.
	struct ParagraphStyle {
	public:
		TextAlign _text_align = TextAlign::Leading;
		ParagraphAlign _paragraph_align = ParagraphAlign::Top;
		FlowDirection _flow_direction = FlowDirection::TopToBottom;
		ReadDirection _read_direction = ReadDirection::LeftToRight;
		WordWrap _word_wrap = WordWrap::Wrap;
		//LengthTag _line_height = length_auto;	// pct is relative to the font size.
		//uchar _indent = 0;					// The indent for each paragraph.
		//uchar _paragraph_space = 0;
	public:
		constexpr ParagraphStyle& text_align(TextAlign text_align) { _text_align = text_align; return *this; }
		constexpr ParagraphStyle& paragraph_align(ParagraphAlign paragraph_align) { _paragraph_align = paragraph_align; return *this; }
		constexpr ParagraphStyle& flow_direction(FlowDirection text_direction) { _flow_direction = text_direction; return *this; }
		constexpr ParagraphStyle& read_direction(ReadDirection read_direction) { _read_direction = read_direction; return *this; }
		constexpr ParagraphStyle& word_wrap(WordWrap word_wrap) { _word_wrap = word_wrap; return *this; }
		//constexpr ParagraphStyle& line_height(LengthTag line_height) { _line_height = line_height; return *this; }
		//constexpr ParagraphStyle& indent(uchar indent) { _indent = indent; return *this; }
		//constexpr ParagraphStyle& paragraph_space(uchar paragraph_space) { _paragraph_space = paragraph_space; return *this; }
	}paragraph;

	// Default font format.
	struct FontFormat {
	public:
		const Ref wchar* _family = L"Arial";
		// const Ref FontCollection* font_collection;
		const Ref wchar* _locale = L"";
		FontWeight _weight = FontWeight::Normal;
		FontStyle _style = FontStyle::Normal;
		FontStretch _stretch = FontStretch::Normal;
		ushort _size = 16;

		Color32 _color = ColorSet::Black;  // Drawing effect.
		bool _underline = false;
		bool _strikeline = false;

		// Letter space and word space.

	public:
		constexpr FontFormat& family(const Ref wchar* family) { _family = family; return *this; }
		constexpr FontFormat& locale(const Ref wchar* locale) { _locale = locale; return *this; }
		constexpr FontFormat& weight(FontWeight weight) { _weight = weight; return *this; }
		constexpr FontFormat& style(FontStyle style) { _style = style; return *this; }
		constexpr FontFormat& stretch(FontStretch stretch) { _stretch = stretch; return *this; }
		constexpr FontFormat& size(ushort size) { _size = size; return *this; }
		constexpr FontFormat& color(Color32 color) { _color = color; return *this; }
		constexpr FontFormat& underline(bool underline) { _underline = underline; return *this; }
		constexpr FontFormat& strikeline(bool strikeline) { _strikeline = strikeline; return *this; }
	}font;
};


END_NAMESPACE(WndDesign)
