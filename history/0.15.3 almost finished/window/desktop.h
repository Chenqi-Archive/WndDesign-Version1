#pragma once

#include "wnd.h"

BEGIN_NAMESPACE(WndDesign)


AbstractInterface IDesktop : virtual IWnd {
private:
	// Disgarded functions.
	void SetRegion() {}
	void SetBackground() {}
	void SetOpacity() {}
	void ConvertToParentPoint() {}
	void SetHandler() {}
	void SetCapture(int) {}
	void SetFocus(int) {}

protected:
	// You should never do delete operation on an Interface. Use Destroy() instead.
	~IDesktop() {}

public:
	using StyleType = int;

	// IDesktop::Create is used for consistency, the parameters has no actual use.
	WNDDESIGN_API static Ref<IDesktop*> Create(Ref<IWndBase*> parent = nullptr, 
											   const StyleType& style = StyleType(), 
											   Ref<ObjectBase*> object = nullptr);

	static Ref<IDesktop*> Get() { return Create(); }

	// Block message.

	virtual int MessageLoop() pure;  // Enter the system message loop.
};


END_NAMESPACE(WndDesign)
