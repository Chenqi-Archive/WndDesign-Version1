#pragma once

#include "textbox_style.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::vector;


// Editbox could support multiple formats.
// The format ranges are dependent with text. If text string changes, the format range should also change.
struct EditBoxStyle : TextBoxStyle {
private:
	struct FormatRange {
		FontFormat font;
		uint begin = 0;
		uint length = length_max;
	};
	vector<FormatRange> _format_ranges;
	
	// May support inline objects.

public:
	void 

};


END_NAMESPACE(WndDesign)