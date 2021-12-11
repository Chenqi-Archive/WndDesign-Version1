#pragma once

#include "../common/core.h"
#include "../geometry/point.h"

BEGIN_NAMESPACE(WndDesign)


enum class Cursor {
	Arrow,			// IDC_ARROW
	Hand,			// IDC_HAND
	Text,		    // IDC_IBEAM
	Move,			// IDC_SIZEALL
	Help,			// IDC_HELP
	Crosshair,		// IDC_CROSS
	Wait,			// IDC_WAIT
	Progress,		// IDC_APPSTARTING
	No,				// IDC_NO
	Resize0,		// IDC_SIZEWE
	Resize45,		// IDC_SIZENESW
	Resize90,		// IDC_SIZENS
	Resize135,	    // IDC_SIZENWSE
	Default = Arrow,
};

// Set the style of the cursor.
void SetCursor(Cursor cursor);

// Get the position of the cursor on the desktop.
Point GetCursorPoint();


END_NAMESPACE(WndDesign)