#pragma once

#include "wnd.h"

BEGIN_NAMESPACE(WndDesign)


struct Desktop : virtual Wnd {
private:
	// Disgarded functions.
	void SetRegion() {}
	void SetBackground() {}
	void SetOpacity() {}
	void ConvertToParentPoint() {}
	void SetHandler() {}
	void SetCapture(int) {}
	void SetFocus(int) {}

public:
	// Block message.
};


extern WNDDESIGN_API Desktop* desktop;


END_NAMESPACE(WndDesign)
