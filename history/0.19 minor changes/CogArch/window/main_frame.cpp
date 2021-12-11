#include "main_frame.h"

inline MainFrame::Style::Style() {
	size.normal(800px, 480px).min(300px, 200px).max(100pct, 100pct);
	position.setHorizontalCenter().setVerticalCenter();
	border.width(5px).color(color_transparent);
	background.setColor(ColorSet::AliceBlue);
	entire_region = region_infinite_tag;
}

MainFrame::MainFrame() :
	ScrollWnd(desktop, Style()),

	_border_resizer(*this),

	_menu_bar(*this, L"Untitled"),
	_caret_blink_timer(*this) {
}

bool MainFrame::Handler(Msg msg, Para para) {
	if (_border_resizer.TrackMsg(msg, para) == true) {
		return false;  // Do not send the message to child window.
	}

	if (IsMouseMsg(msg)) {
		OnMouseMsg(msg, para);
		StartBlinkingCaret();
	} 
	
	else if (IsKeyboardMsg(msg)) {
		OnKeyboardMsg(msg, para);
		StartBlinkingCaret();
	} 
	
	else if (msg == Msg::Timer) {
		BlinkCaret();
	}
	
	else if (msg == Msg::LoseFocus) {
		HideCaret();
	}
	return true;
}

void MainFrame::OnMouseMsg(Msg msg, Para para) {
	const MouseMsg& mouse_msg = GetMouseMsg(para);
	switch (msg) {
	case Msg::LeftDown:
		(*this)->SetCapture();
		(*this)->SetFocus();
		_has_mouse_down = true;
		_has_mouse_dragged = false;
		_mouse_down_position = mouse_msg.point;
		break;
	case Msg::MouseMove:
		if (_has_mouse_down) {
			Point current_point = mouse_msg.point;
			Vector moving_vector = _mouse_down_position - current_point;
			if (moving_vector != vector_zero) {
				SetCursor(Cursor::Move);
				_has_mouse_dragged = true;
				_mouse_down_position = current_point;
				(*this)->ScrollView(moving_vector);
			}
		} else {
			SetCursor(Cursor::Text);
		}
		break;
	case Msg::LeftUp:
		_has_mouse_down = false;
		if (!_has_mouse_dragged) { SetCaret(mouse_msg.point); }
		(*this)->ReleaseCapture();
		break;
	}
}

void MainFrame::OnKeyboardMsg(Msg msg, Para para) {
	// When receives character/string/IME message, create a new Block at current caret position,
	//   hide the caret, and resend the message to the newly created Block.
	if (msg == Msg::KeyDown) {
		// Convert the control key messages to char messages.
		switch (GetKeyMsg(para).key) {
		case Key::Enter: OnKeyboardMsg(Msg::Char, reinterpret_cast<Para>(L'\n')); break;
		case Key::Tab: OnKeyboardMsg(Msg::Char, reinterpret_cast<Para>(L'\t')); break;
		default: break;
		}
	}
	else if (_caret_state != CaretState::Hide && 
		((msg == Msg::Char && GetCharMsg(para) != 0) || msg == Msg::ImeStartComposition)) {
		// Create a new block.
		TextBlock& new_block = _blocks.emplace_front(*this, _caret_point + Vector(-3, -5));
		new_block.SetIterator(_blocks.begin());
		(*this)->AddChildStatic(new_block.GetWnd());
		// Set focus so that all subsequent keyboard messages will be sent to the block.
		// And myself will receive Msg::LoseFocus at the same time, when the temporary caret will be hidden.
		new_block->SetFocus(); 
		// Resend the message to the block.
		new_block.Handler(msg, para);  
	}
}

void MainFrame::AppendFigure(FigureQueue& figure_queue) {
	// Draw caret.
	if (_caret_state == CaretState::Show || _caret_state == CaretState::BlinkShow) {
		figure_queue.Push(ColorFigure::Create(caret_color), GetCaretRegion());
	}
}

void MainFrame::SetCaret(Point mouse_click_point) {
	_caret_state = CaretState::Hide;
	(*this)->UpdateRegion(GetCaretRegion());
	_caret_point = (*this)->GetScrollPosition() + (mouse_click_point - point_zero);
	_caret_state = CaretState::Show;
	(*this)->UpdateRegion(GetCaretRegion());
}

void MainFrame::StartBlinkingCaret() {
	if (_caret_state != CaretState::Hide) {
		if (!_caret_blink_timer.IsSet()) {
			_caret_blink_timer.Set(caret_blink_period);
		}
		_caret_blink_time = 0;
	}
}

void MainFrame::BlinkCaret() {
	_caret_blink_time += caret_blink_period;
	if (_caret_blink_time >= caret_blink_expire_time) {
		_caret_state = CaretState::Show;
		_caret_blink_timer.Stop();
		return;
	}

	if (_caret_state == CaretState::Hide) {
		_caret_blink_timer.Stop();
	} else if (_caret_state == CaretState::Show || _caret_state == CaretState::BlinkShow) {
		_caret_state = CaretState::BlinkHide;
		(*this)->UpdateRegion(GetCaretRegion());
	} else { // _caret_state = CaretState::BlinkHide.
		_caret_state = CaretState::BlinkShow;
		(*this)->UpdateRegion(GetCaretRegion());
	}
}

void MainFrame::HideCaret() {
	_caret_state = CaretState::Hide;
	(*this)->UpdateRegion(region_infinite);
}
