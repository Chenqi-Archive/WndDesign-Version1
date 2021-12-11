#pragma once

#include "../common/core.h"
#include "../common/msg.h"
#include "../style/button_style.h"

BEGIN_NAMESPACE(WndDesign)

// Receives mouse messages: MouseMove, LeftDown, LeftUp only.

struct Wnd;

struct Button{
	static Alloc Button* Create(Ref Wnd* parent, const Ref ButtonStyle* style, Handler handler);

	virtual void SetStyle(const Ref ButtonStyle* style);
};

END_NAMESPACE(WndDesign)
