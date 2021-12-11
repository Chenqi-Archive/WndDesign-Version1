#pragma once

#include "wnd.h"
#include "wnd_base_impl.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::list;

class _Wnd_Impl : virtual public Wnd, virtual public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
protected:
	MultipleWndLayer _base_layer;
	list<SingleWndLayer> _top_layers;

public:
	// Set the canvas, and then allocate canvases for child windows.
	void SetCanvas(const Canvas& canvas) override;
	void Draw() const override;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<Ref WndBase*> child_wnds;

public:
	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;
};


END_NAMESPACE(WndDesign)