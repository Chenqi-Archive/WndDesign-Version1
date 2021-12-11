#include "button_impl.h"
#include "wnd.h"
#include "../style/style_helper.h"

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc Button* Button::Create(Ref WndBase* parent, const ButtonStyle& style, Handler handler, const wstring& text) {
	_Button_Impl* button = new _Button_Impl(new ButtonStyle(style), text, handler);
	button->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(button); }
	return button;
}



_Button_Impl::_Button_Impl(Alloc ButtonStyle* style, const wstring& text, Handler handler) :
	_TextBox_Impl(style, text, handler), _current_background(style->background), _state(State::Normal){
}

_Button_Impl::~_Button_Impl() {}

void _Button_Impl::Composite(Rect region_to_update) const {
	const ButtonStyle& style = static_cast<const ButtonStyle&>(GetStyle());
	FigureQueue figure_queue;
	Figure* background = new RoundedRectangle(style.border._radius,
											  style.border._width,
											  style.border._color,
											  _current_background);
	figure_queue.Push(background, Rect(point_zero, GetSize()));
	if (!_text.empty()) {
		Figure* text = new TextFigure(_text, style);
		figure_queue.Push(text, CalculateTextRegion(style, GetSize()));
	}
	DrawFigureQueue(figure_queue, region_to_update);
}

bool _Button_Impl::DispatchMessage(Msg msg, void* para) {
	const ButtonStyle& style = static_cast<const ButtonStyle&>(GetStyle());
	if (msg == Msg::MouseMove) {
		if (_state == State::Normal) {
			_state = State::Hover;
			_current_background = style.background_hover;
			RegionUpdated(region_infinite);
		}
	}
	if (msg == Msg::MouseLeave) {
		_state = State::Normal;
		_current_background = style.background;
		RegionUpdated(region_infinite);
	}
	if (msg == Msg::LeftDown) {
		_state = State::Down;
		_current_background = style.background_down;
		RegionUpdated(region_infinite);
	}
	if (msg == Msg::LeftUp) {
		if (_state == State::Down) {
			_state = State::Hover;
			_current_background = style.background_hover;
			RegionUpdated(region_infinite);
		}
	}
	return _WndBase_Impl::DispatchMessage(msg, para);
}

END_NAMESPACE(WndDesign)
