#pragma once

#include "../common/core.h"
#include "../style/wnd_style.h"
#include "../common/msg.h"

BEGIN_NAMESPACE(WndDesign)

struct Wnd {
	static Alloc Wnd* Create(Ref Wnd* parent, const Ref WndStyle* style, Handler handler);

	virtual void SetStyle(const Ref WndStyle* style) pure;
	virtual void SetHandler(Handler handler) pure;

	virtual void Show() pure;
	virtual void Destroy() pure;
};

extern Ref Wnd* desktop;

END_NAMESPACE(WndDesign)
