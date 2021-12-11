#include "wnd_base_impl.h"
#include "wnd_impl.h"

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc WndBase* WndBase::Create(Ref Wnd* parent, const WndStyle& style, Handler handler) {
	WndBase* wnd = new _WndBase_Impl(dynamic_cast<_WndBase_Impl*>(parent), style, handler);
	if (parent != nullptr) { parent->AddChild(wnd); }
	return wnd;
}

WNDDESIGN_API void DestroyWnd(Alloc WndBase* wnd) {
	delete wnd;
}



_WndBase_Impl::_WndBase_Impl(Ref _WndBase_Impl* parent, const WndStyle& style, Handler handler) :
	_parent(parent),
	_user_data(0),
	_style(style),
	_handler(handler),
	_canvas(),
	_is_redrawing_layer(false),
	_focus_wnd(nullptr){
}

_WndBase_Impl::~_WndBase_Impl() {}

void _WndBase_Impl::SetUserData(Data data) { _user_data = data; }

WndBase::Data _WndBase_Impl::GetUserData() const { return _user_data; }

void _WndBase_Impl::SetSize(LengthTag width, LengthTag height) {
	_style.size.normal(width, height);
	_Wnd_Impl* parent = dynamic_cast<_Wnd_Impl*>(_parent);
	if (parent != nullptr) {
		parent->MoveChild(this);
	}
}

void _WndBase_Impl::SetPosition(LengthTag left, LengthTag top) {
	_style.position.left(left).top(top);
	_Wnd_Impl* parent = dynamic_cast<_Wnd_Impl*>(_parent);
	if (parent != nullptr) {
		parent->MoveChild(this);
	}
}

void _WndBase_Impl::SetOpacity(uchar opacity) {
	_style.render.opacity(opacity);
	_Wnd_Impl* parent = dynamic_cast<_Wnd_Impl*>(_parent);
	if (parent != nullptr) {
		parent->SetChildRenderStyle(this);
	}
}

void _WndBase_Impl::SetBackground(Color32 background) {
	_style.background = background;
	Composite();
}

Point _WndBase_Impl::ConvertToParentPoint(Point point) { // Get the point coordinate as in parent window.
	return _canvas.ConvertToParentPoint(point);
}

void _WndBase_Impl::Composite(Rect region_to_update, const Ref Layer* layer) const {
	FigureQueue figure_queue;
	Figure* background = new RoundedRectangle(_style.border._radius,
											  _style.border._width,
											  _style.border._color,
											  _style.background);
	figure_queue.Push(background, Rect(point_zero, _canvas.GetSize()).Shrink(_style.border._width / 2));
	_canvas.DrawFigureQueue(figure_queue);
}

void _WndBase_Impl::LayerUpdated(const Ref Layer* layer, Rect region) {
	// Do nothing.
}


WNDDESIGN_API void SendMsg(Ref WndBase* wnd, Msg msg, Para para) {
	Ref _WndBase_Impl* the_wnd = dynamic_cast<Ref _WndBase_Impl*>(wnd);
	the_wnd->DispatchMessage(msg, para);
}


END_NAMESPACE(WndDesign)