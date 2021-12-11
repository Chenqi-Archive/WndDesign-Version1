#pragma once

#include "textbox_style.h"

#include <vector>

BEGIN_NAMESPACE(WndDesign)

using std::vector;


// TextBox style with multiple formats.
struct TextBoxStyleEx : TextBoxStyle {
private:
	struct FormatRange {
		FontFormat font;
		uint begin = 0;
		uint length = length_max;
	};
	vector<FormatRange> _format_ranges;
	
	// May support inline objects.
};


END_NAMESPACE(WndDesign)