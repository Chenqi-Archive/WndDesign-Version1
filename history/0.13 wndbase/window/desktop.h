#pragma once

#include "wnd.h"

BEGIN_NAMESPACE(WndDesign)


struct Desktop : virtual Wnd {
private:
	// Disgarded functions.
	void SetHandler(Handler handler) {}
	void SetSize(LengthTag width, LengthTag height) {}
	void SetPosition(LengthTag left, LengthTag top) {}
	void SetBackground(Color32 background) {}
	void SetOpacity(uchar opacity) {}
	Point ConvertToParentPoint(Point point) { return point_zero; }
	void SetUserData(Data data) {}
	Data GetUserData() const { return 0; }

public:
	// Add a new dialog window and block the message input to parent window until the dialog is closed.
	virtual void AddModalDialogBox(const Ref WndBase* dialog, const Ref WndBase* parent) pure;
};

extern WNDDESIGN_API Desktop* desktop;

END_NAMESPACE(WndDesign)
