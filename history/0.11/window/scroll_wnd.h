#pragma once

#include "../common/core.h"
#include "../common/msg.h"
#include "../style/scroll_wnd_style.h"


BEGIN_NAMESPACE(WndDesign)

struct Wnd;

struct ScrollWnd{
	static Alloc ScrollWnd* Create(Ref Wnd* parent, const Ref ScrollWndStyle* style, Handler handler);

	virtual void SetStyle(const Ref ScrollWndStyle* style) pure;

	// Set the current display position.
	virtual void SetPosition(const Point& point) pure;
	
	// Move the current display position by the vector.
	virtual void MovePosition(const Vector& vector) pure;


	// Zoom in or out.
	//virtual void ScaleView(Point center, float ratio) pure;
};


END_NAMESPACE(WndDesign)
