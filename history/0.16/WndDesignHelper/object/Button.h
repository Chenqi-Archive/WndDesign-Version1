#pragma once

#include "WndTypeBase.h"


BEGIN_NAMESPACE(WndDesignHelper)


struct ButtonStyle : TextBoxStyle {
	Color background_hover;
	Color background_down;
};


class Button : public TextBox {
private:
	enum class State { Normal, Hover, Down } _state = State::Normal;
	Color _background_normal = ColorSet::White;
	Color _background_hover = ColorSet::White;
	Color _background_down = ColorSet::White;

public:
	void Init(Ref<ObjectBase*> parent, const ButtonStyle& style) {
		_background_normal = style.background;
		_background_hover = style.background_hover;
		_background_down = style.background_down;
		_state = State::Normal;
		TextBox::Init(parent, style);
	}
	void Destroy() { TextBox::Destroy(); }
	bool Handler(Ref<IWndBase*> wnd, Msg msg, Para para) override {
		if (wnd != GetWnd()) { return false; }
		if (msg == Msg::MouseMove) {
			if (_state == State::Normal) {
				_state = State::Hover;
				GetWnd()->SetBackground(_background_hover);
			}
			return true;
		}
		if (msg == Msg::MouseLeave) {
			_state = State::Normal;
			GetWnd()->SetBackground(_background_normal);
			return true;
		}
		if (msg == Msg::LeftDown) {
			_state = State::Down;
			GetWnd()->SetBackground(_background_down);
			return true;
		}
		if (msg == Msg::LeftUp) {
			if (_state == State::Down) {
				_state = State::Hover;
				GetWnd()->SetBackground(_background_hover);
			}
			return true;
		}
		return false;
	}
};


END_NAMESPACE(WndDesignHelper)