#pragma once

#include "../common/core.h"
#include "../common/feature.h"
#include "../message/msg.h"

BEGIN_NAMESPACE(WndDesign)
struct IWndBase;
class FigureQueue;
END_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(WndDesignHelper)

using namespace WndDesign;


// A convenient wrapper class for window objects.
AbstractInterface ObjectBase : private Uncopyable{
protected:
	ObjectBase() {}
	~ObjectBase() {}
public:
	// Called when the window is compositing, so that user can draw custom figures on the window.)		
	virtual bool Handler(Msg msg, Para para) { return false; }
	virtual void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) {}
	virtual Ref<const IWndBase*> GetWnd() const { return nullptr; }
	Ref<IWndBase*> GetWnd() { return const_cast<Ref<IWndBase*>>(const_cast<const ObjectBase&>(*this).GetWnd()); }
	Ref<const IWndBase*> operator->() const { return GetWnd(); }
	Ref<IWndBase*> operator->() { return GetWnd(); }
};


END_NAMESPACE(WndDesignHelper)