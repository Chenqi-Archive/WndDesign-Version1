
	//// Caret ////
private:
	static const uint caret_blink_period = 500;  // 500ms
	static const uint max_caret_blink_time = 10000;  // 10s

	Timer _timer;
	uint _current_caret_blink_time;
	void StartBlinking();
	void StopBlinking();

	enum class CaretState { None, Static, BlinkShow, BlinkHide } _caret_state;
	uint _caret_position;
	uint _caret_position_in_line;
	Rect _caret_region;

	void SetCaret();
	void DestroyCaret();
	void UpdateCaretState(CaretState caret_state) {
		_caret_state = caret_state;
	}

	enum class MoveCaretDirection { Left, Up, Right, Down };
	void MoveCaret(MoveCaretDirection direction);


	//// Selection ////
private:
	uint _selection_begin;
	uint _selection_end;
	vector<Rect> _selection_regions;

	void SelectText(Point mouse_move_position);
	void UpdateSelectionState();


	//// Edit ////
private:
	uint _edit_position;
	uint _edit_length;
