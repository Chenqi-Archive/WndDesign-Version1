#pragma once

#include "WndTypeBase.h"


BEGIN_NAMESPACE(WndDesignHelper)


struct ButtonStyle : TextBoxStyle {
	Background background_hover;
	Background background_down;
};


// A simple button implementation based on WndBase(or Textbox to set text).
template <class BaseType>
class ButtonBase : public BaseType {
private:
	enum class State { Normal, Hover, Down } _state;
	Background _background_normal;
	Background _background_hover;
	Background _background_down;
public:
	using BaseType::GetWnd;
	ButtonBase(const ObjectBase& parent, const ButtonStyle& style):
		BaseType(parent, style),
		_state(State::Normal),
		_background_normal(style.background),
		_background_hover(style.background_hover),
		_background_down(style.background_down) {
	}
	bool Handler(Msg msg, Para para) override {
		if (msg == Msg::MouseMove) {
			if (_state == State::Normal) {
				_state = State::Hover;
				(*this)->SetBackground(_background_hover);
			}
		}
		else if (msg == Msg::MouseLeave) {
			_state = State::Normal;
			(*this)->SetBackground(_background_normal);
		}
		else if (msg == Msg::LeftDown) {
			_state = State::Down;
			(*this)->SetBackground(_background_down);
		}
		else if (msg == Msg::LeftUp) {
			if (_state == State::Down) {
				OnClick();
				_state = State::Hover;
				(*this)->SetBackground(_background_hover);
			}
		}
		return true;
	}
	virtual void OnClick() {}
};

using Button = ButtonBase<WndBase>;
using ButtonWithText = ButtonBase<TextBox>;


END_NAMESPACE(WndDesignHelper)