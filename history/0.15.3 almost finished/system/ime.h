#pragma once

#include "../common/core.h"
#include "../figure/color.h"
#include "../geometry/geometry.h"

#include <string>
#include <vector>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;
using std::vector;


struct UnderlineStyle {
	uint begin = 0;
	uint end = 0;
	Color color = ColorSet::White;
	bool thick = false;
};

// The status of an ongoing composition.
struct ImeComposition {
	uint begin = 0;	// The start position of the selection range.
	uint end = 0;	// The end of the selection range.
	enum class Type: uint { None = 0, CompositionString = 0x0008, ResultString = 0x0800 } type = Type::None;
	wstring string;					   // The string retrieved.
	vector<UnderlineStyle> underlines; // The underline information of the composition string.
};


struct IWndBase;

// Enables the IME attached to the given window.
void EnableIME(Ref<IWndBase*> wnd);
// Disables the IME attached to the given window.
void DisableIME(Ref<IWndBase*> wnd);
// Cancels an ongoing composition of the IME attached to the given window.
void CancelIME(Ref<IWndBase*> wnd);

// Move the current caret region when using an IME input. (caret_region is relative to the window)
void MoveCaret(Ref<IWndBase*> wnd, Rect caret_region);



END_NAMESPACE(WndDesign)