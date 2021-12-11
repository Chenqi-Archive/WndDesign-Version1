#pragma once

#include "wnd.h"

BEGIN_NAMESPACE(WndDesign)


AbstractInterface IDesktop : virtual IWndBase {
private:
	// Disgarded functions.
	void SetBackground() {}
	void SetOpacity() {}
	void SetRegion() {}
	void ConvertToLayerPoint() {}
	void ConvertToParentPoint() {}
	void SetHandler() {}
	void SetCapture(int) {}
	void SetFocus(int) {}
protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IDesktop() {}

public:
	WNDDESIGN_API static IDesktop& Get();
	virtual int MessageLoop() pure;  // Enter the system message loop.
};


END_NAMESPACE(WndDesign)