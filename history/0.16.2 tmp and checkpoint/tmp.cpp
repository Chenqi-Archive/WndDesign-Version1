
void _TextBox_Impl::StartBlinking() {
	if (_caret_state == CaretState::None) { return; }
	if (_timer == nullptr) {
		_timer = SetTimerSync(caret_blink_period, CaretBlinkTimerProc, this);
	}
	_current_caret_blink_time = 0;
}

void _TextBox_Impl::StopBlinking() {
	if (_timer == nullptr) { return; }
	KillTimerSync(_timer); _timer = nullptr;
	UpdateCaretState(CaretState::Static);
}

void _TextBox_Impl::SetCaret() {
	HitTestInfo info = _layout.HitTestPoint(PointToTextPoint(_mouse_down_position));
	_caret_region.point = TextPointToPoint(info.geometry_region.point);
	if (info.is_trailing_hit) { _caret_region.point.x += static_cast<int>(info.geometry_region.size.width); }
	_caret_region.size = Size(1, info.geometry_region.size.height);

	_selection_regions.clear();
	_caret_position = info.text_position + (info.is_trailing_hit ? info.text_length : 0);

	UpdateCaretState(CaretState::BlinkShow);
}

void _TextBox_Impl::DestroyCaret() {
	_caret_state = CaretState::None;
}

void _TextBox_Impl::SelectText(Point mouse_move_position) {
	HitTestInfo info = _layout.HitTestPoint(PointToTextPoint(mouse_move_position));

	// Update the caret position to the current mouse position.
	_caret_region.point = TextPointToPoint(info.geometry_region.point);
	if (info.is_trailing_hit) { _caret_region.point.x += static_cast<int>(info.geometry_region.size.width); }
	_caret_region.size = Size(1, info.geometry_region.size.height);

	uint selection_begin = _caret_position;
	uint selection_end = info.text_position + (info.is_trailing_hit ? info.text_length : 0);

	if (selection_begin == selection_end) { return; }
	if (selection_begin > selection_end) {
		std::swap(selection_begin, selection_end);
	}

	_layout.HitTestTextRange(selection_begin, selection_end - selection_begin, _selection_regions);
	UpdateSelectionState();
}

void _TextBox_Impl::UpdateSelectionState() {
	if (_selection_regions.empty()) { return; }
	Rect region_to_update = region_empty;
	for (auto it : _selection_regions) {
		region_to_update = region_to_update.Union(TextRegionToRegion(it));
	}
	RegionUpdated(region_infinite);
}

bool _TextBox_Impl::DispatchMessage(Msg msg, Para para) {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());

	// For drawing caret and selecting text.
	if (!style.action._enable_select) { return _WndBase_Impl::DispatchMessage(msg, para); }
	const MouseMsg& mouse_msg = GetMouseMsg(para);
	if (msg == Msg::LeftDown) {
		SetCapture();
		SetFocus();
		_has_mouse_down = true;
		_mouse_down_position = mouse_msg.point;
		SetCaret();
		StartBlinking();
		return true;
	}
	if (msg == Msg::MouseMove) {
		if (_has_mouse_down) {
			SelectText(mouse_msg.point);
		}
		if (_text_region.Contains(mouse_msg.point)) {
			SetCursor(Cursor::Text);
		} else {
			SetCursor();
		}
		return true;
	}
	if (msg == Msg::LeftUp) {
		_has_mouse_down = false;
		ReleaseCapture();
		return true;
	}
	if (msg == Msg::Timer) {
		if (_caret_state == CaretState::BlinkShow) { UpdateCaretState(CaretState::BlinkHide); } else if (_caret_state == CaretState::BlinkHide) { UpdateCaretState(CaretState::BlinkShow); }
		_current_caret_blink_time += caret_blink_period;
		if (_current_caret_blink_time >= max_caret_blink_time) {
			StopBlinking();
		}
		return true;
	}

	// For editing.
	if (!style.action._enable_edit) { return _WndBase_Impl::DispatchMessage(msg, para); }

	if (msg == Msg::KeyDown) {
		KeyMsg key_msg = GetKeyMsg(para);
		switch (key_msg.key) {
		case Key::Left:
		case Key::Up:
		case Key::Right:
		case Key::Down:

		case Key::Backspace:
		case Key::Delete:
		case Key::Home:
		case Key::End:

		case Key::Enter:
		case Key::Tab:

		default: break;
		}
	}
	if (msg == Msg::Char) {
		if (wchar ch = GetCharMsg(para)) {

		}
	}

	if (msg == Msg::ImeStartComposition) {
		// Save the current caret position.
		return true;
	}
	if (msg == Msg::ImeComposition) {
		// Replace text

		const ImeComposition& composition = GetImeCompositionMsg(para);

	}
	if (msg == Msg::ImeEndComposition) {
		const ImeComposition& result = GetImeCompositionMsg(para);

		// Confirm text change. 
	}


	return _WndBase_Impl::DispatchMessage(msg, para);
}

void EditBox::Composite(FigureQueue& figure_queue) {
	// Draw selection.
	if (style.action._enable_select) {
		for (auto it : _selection_regions) {
			figure_queue.Push(new ColorFigure(style.action._selection_color), TextRegionToRegion(it));
		}
	}
	// Draw caret.
	if (style.action._show_caret && (_caret_state == CaretState::Static || _caret_state == CaretState::BlinkShow)) {
		figure_queue.Push(new ColorFigure(style.action._caret_color), _caret_region);
	}
}
