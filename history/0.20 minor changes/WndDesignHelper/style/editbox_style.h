#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


struct EditBoxStyle : public TextBoxStyle {
	struct EditStyle {
	public:
		Color _selection_color = Color(ColorSet::DimGray, 0x7f);
		Color _caret_color = ColorSet::DimGray;
		bool _disable_select = false;
		bool _disable_caret = false;
		bool _disable_copy = false;
		bool _disable_edit = false;
	public:
		constexpr EditStyle& selection_color(Color selection_color) { _selection_color = selection_color; return *this; }
		constexpr EditStyle& caret_color(Color caret_color) { _caret_color = caret_color; return *this; }
		constexpr EditStyle& disable_select() { _disable_select = true; return *this; }
		constexpr EditStyle& disable_caret() { _disable_caret = true; return *this; }
		constexpr EditStyle& disable_copy() { _disable_copy = true; return *this; }
		constexpr EditStyle& disable_edit() { _disable_edit = true; return *this; }
	}edit;
};


END_NAMESPACE(WndDesign)