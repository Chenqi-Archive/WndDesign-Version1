#include "EditBox.h"


BEGIN_NAMESPACE(WndDesignHelper)


void EditBox::AppendFigure(FigureQueue& figure_queue) {
	// Draw caret.
	if (_caret_state == CaretState::Show) {
		figure_queue.Push(ColorFigure::Create(_edit_style._caret_color), _caret_region);
	}
	// Draw selection.
	if (HasSelection()) {
		for (auto& it : _selection_info) {
			figure_queue.Push(ColorFigure::Create(_edit_style._selection_color), it.geometry_region);
		}
	}
}

bool EditBox::OnMouseMsg(Msg msg, Para para) {
	const MouseMsg& mouse_msg = GetMouseMsg(para);
	switch (msg) {
	case Msg::LeftDown:
		(*this)->SetCapture();
		(*this)->SetFocus();
		_has_mouse_down = true;
		_mouse_down_position = mouse_msg.point;
		SetCaret(_mouse_down_position);
		break;
	case Msg::MouseMove:
		if (_has_mouse_down) {
			DoSelection(mouse_msg.point);
		}
		UpdateCursor(mouse_msg.point);
		break;
	case Msg::LeftUp:
		_has_mouse_down = false;
		(*this)->ReleaseCapture();
		break;
	default:
		break;
	}
	
	return false;
}

bool EditBox::OnKeyboardMsg(Msg msg, Para para) {
	switch (msg) {
	// For virtual key message.
	case Msg::KeyDown:
		switch (GetKeyMsg(para).key) {
		case Key::Left: MoveCaret(MoveCaretDirection::Left); break;
		case Key::Right: MoveCaret(MoveCaretDirection::Right); break;
		case Key::Up: MoveCaret(MoveCaretDirection::Up); break;
		case Key::Down: MoveCaret(MoveCaretDirection::Down); break;
		case Key::Home: MoveCaret(MoveCaretDirection::Home); break;
		case Key::End: MoveCaret(MoveCaretDirection::End); break;

		case Key::Enter: Insert(L'\n'); break;
		case Key::Tab: Insert(L'\t'); break;

		case Key::Backspace: Delete(true); break;
		case Key::Delete: Delete(false); break;
		default: break;
		}
		break;
	case Msg::Char: Insert(GetCharMsg(para)); break;

	// For IME message.
	case Msg::ImeStartComposition: ImeStartComposition(); break;
	case Msg::ImeComposition: // Fall through.
	case Msg::ImeEndComposition: ImeComposition(GetImeCompositionMsg(para).string); break;

	default: return false;
	}
	return true;
}

bool EditBox::Handler(Msg msg, Para para) {
	if (IsMouseMsg(msg)) { return OnMouseMsg(msg, para); }
	if (IsKeyboardMsg(msg)) { return OnKeyboardMsg(msg, para); }
	if (msg == Msg::LoseFocus) { HideCaret(); }
	return false;
}


void EditBox::UpdateCaret(const HitTestInfo& info) {
	// Set caret position.
	_caret_text_position = info.text_position;

	// Set caret region.
	_caret_region.point = info.geometry_region.point;
	_caret_region.size = Size(caret_width, info.geometry_region.size.height);

	// If the trailing of a character is hit, add the length of the character.
	if (info.is_trailing_hit) { 
		_caret_text_position += info.text_length;
		_caret_region.point.x += static_cast<int>(info.geometry_region.size.width); 
	}

	_caret_state = CaretState::Show;
}

uint EditBox::GetCharacterLength(uint text_position) {
	// One could directly get the length of the character by checking the utf-16 encoding 
	//   rather than doing hit test. But I don't know much about the encoding. 
#pragma message("Use utf-16 encoding information to decide the length of the Character.")
	HitTestInfo info = (*this)->HitTestTextPosition(text_position);
	return info.text_length;
}

void EditBox::SetCaret(Point mouse_down_position) {
	HitTestInfo info = (*this)->HitTestPoint(mouse_down_position);
	UpdateCaret(info);

	// Clear selection and reset selection range.
	_selection_info.clear();
	_selection_begin = _selection_end = _caret_text_position;

	(*this)->UpdateRegion(region_infinite);
}

void EditBox::SetCaret(uint text_position, bool is_trailing_hit) {
	HitTestInfo info = (*this)->HitTestTextPosition(text_position);
	// If text_position falls in a large character, and the next character is to be hit,
	//   set is_trailing_hit to be true, so the current character will be skipped over.
	info.is_trailing_hit = is_trailing_hit;
	UpdateCaret(info);

	// Clear selection.
	_selection_info.clear();
	_selection_begin = _selection_end = _caret_text_position;

	(*this)->UpdateRegion(region_infinite);
}

void EditBox::HideCaret() {
	_caret_state = CaretState::Hide;
	(*this)->UpdateRegion(region_infinite);
}

void EditBox::ShowCaret() {
	_caret_state = CaretState::Show;
	(*this)->UpdateRegion(region_infinite);
}

void EditBox::MoveCaret(MoveCaretDirection direction) {
	switch (direction) {
	case MoveCaretDirection::Left:
		if (HasSelection()) {
			SetCaret(_selection_begin, false);
		} else {
			SetCaret(_caret_text_position - 1, false);
		}
		break;
	case MoveCaretDirection::Right:
		if (HasSelection()) {
			SetCaret(_selection_end, false);
		} else {
			SetCaret(_caret_text_position, true);
		}
		break;
		// Up, down, Home and End are not supported yet.
	default:
		break;
	}
}


void EditBox::DoSelection(Point mouse_move_position) {
	HitTestInfo info = (*this)->HitTestPoint(mouse_move_position);

	// Move the caret to the current mouse position.
	UpdateCaret(info); 

	// Set selection_end to the current caret position. (selection_begin has been set when mouse down)
	_selection_end = _caret_text_position;
	if (_selection_begin == _selection_end) { return; }
	if (_selection_end < _selection_begin) { std::swap(_selection_begin, _selection_end); }

	// Calculate selected regions.
	(*this)->HitTestTextRange(_selection_begin, _selection_end - _selection_begin, _selection_info);

	(*this)->UpdateRegion(region_infinite);
}

void EditBox::Insert(wchar ch) {
	// Block composition. 
	// TextUpdated() will automatically update the entire window, but the caret region has not be changed.
	// After the caret is reset, UpdateRegion() will be called explicitly.
	(*this)->BlockComposition();  

	// Get the text string to be modified.
	wstring& text = (*this)->ModifyText();

	if (HasSelection()) {
		// If has selection, replace selection with the input character, and move the caret.
		text.replace(_selection_begin, _selection_end - _selection_begin, 1, ch);
		(*this)->TextUpdated();  // Must update text layout before setting caret.
		SetCaret(_selection_begin + 1, false);
	} else {
		// If has no selection, insert the character at the current caret position.
		text.insert(_caret_text_position, 1, ch);
		(*this)->TextUpdated();  // Must update text layout before setting caret.
		SetCaret(_caret_text_position + 1, false);
	}

	// Redraw the entire region.
	(*this)->UpdateRegion(region_infinite);
}

void EditBox::Delete(bool is_backspace) { // Similar to Insert() above.
	(*this)->BlockComposition();
	wstring& text = (*this)->ModifyText();
	if (HasSelection()) {
		// If has selection, delete the selected text.
		text.erase(_selection_begin, _selection_end - _selection_begin);
		(*this)->TextUpdated();  // Must update text layout before setting caret.
		SetCaret(_selection_begin, false);
	} else {
		// If has no selection, delete the former or latter character depending on is_backspace.
		if (is_backspace) {
			if (_caret_text_position == 0) { 
				// At the beginning of the text string. Do nothing.
				(*this)->UpdateRegion(region_empty); // Remove composition blocking.
				return; 
			}
			// Use the caret position of previous text layout, because that even the text  
			//   layout has changed, the text before the deleted charecter will not change.
			// Remember the previous caret position to get the length of the character.
			uint previous_caret_position = _caret_text_position; 
			SetCaret(previous_caret_position - 1, false);
			uint character_length = previous_caret_position - _caret_text_position;
			text.erase(_caret_text_position, character_length);
			(*this)->TextUpdated();
		} else {
			if (_caret_text_position >= text.length()) {
				// At the end of the text string. Do nothing.
				(*this)->UpdateRegion(region_empty); // Remove composition blocking.
				return;
			}
			// No need to reset caret position. But should get the character length.
			uint character_length = GetCharacterLength(_caret_text_position);
			if (character_length == 0) { (*this)->UpdateRegion(region_empty); return; }
			text.erase(_caret_text_position, character_length);
			(*this)->TextUpdated();
		}
	}
	(*this)->UpdateRegion(region_infinite);
}


void EditBox::ImeStartComposition() {
	Rect ime_caret_region;
	if (HasSelection()) {
		_ime_composition_begin = _selection_begin;
		_ime_composition_end = _selection_end;
		ime_caret_region.point = _selection_info.front().geometry_region.point;
		ime_caret_region.size = Size(caret_width, _selection_info.front().geometry_region.size.height);
	} else {
		_ime_composition_begin = _caret_text_position;
		_ime_composition_end = _ime_composition_begin;
		ime_caret_region = _caret_region;
	}

	// Update the ime window position to ime_composition_begin.
	// But note that when my window is moved by parent, the ime window will not be followed until next update.
	MoveImeWindow(GetWnd(), ime_caret_region);
}

void EditBox::ImeComposition(const wstring& composition_string) {
	(*this)->BlockComposition();
	wstring& text = (*this)->ModifyText();
	text.replace(_ime_composition_begin, _ime_composition_end - _ime_composition_begin, composition_string);
	(*this)->TextUpdated();  // Must update text layout before setting caret.
	_ime_composition_end = _ime_composition_begin + static_cast<uint>(composition_string.length());
	SetCaret(_ime_composition_end, false);
	(*this)->UpdateRegion(region_infinite);
}


END_NAMESPACE(WndDesignHelper);