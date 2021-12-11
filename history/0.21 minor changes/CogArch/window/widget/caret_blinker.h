#pragma once

#include "../WndDesign.h"
#include "mouse_click_tracker.h"


// The caret figure for base layer of scrollwnd. (Used by SpaceView)
class CaretBlinker {
private:
	ScrollWnd& _parent;
	MouseClickTrackerWithTolerance _click_tracker;

private:
	inline static const Color caret_color = ColorSet::Gray;
	inline static const Size caret_size = Size(1, 20); // (caret height may depending on the font size)

	// Caret state.
	enum class CaretState { Hide, Show, BlinkHide, BlinkShow } _caret_state = CaretState::Hide;
	Point _caret_point = point_zero;

	// Caret blinking.
	inline static const uint caret_blink_period = 500;   // 500ms
	inline static const uint caret_blink_expire_time = 20000; // 20s. After 20s without any action. The caret will remain showing.
	Timer _caret_blink_timer;
	uint _caret_blink_time = 0;

	void SetCaret(Point point) {
		_caret_state = CaretState::Hide;
		_parent->UpdateRegion(GetCaretRegion());
		_caret_point = _parent->ConvertPointToBaseLayerPoint(point);
		_caret_state = CaretState::Show;
		_parent->UpdateRegion(GetCaretRegion());
	}
	void StartBlinkingCaret() {
		if (_caret_state != CaretState::Hide) {
			if (!_caret_blink_timer.IsSet()) {
				_caret_blink_timer.Set(caret_blink_period);
			}
			_caret_blink_time = 0;
		}
	}
	void BlinkCaret() {
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
			_parent->UpdateRegion(GetCaretRegion());
		} else { // _caret_state = CaretState::BlinkHide.
			_caret_state = CaretState::BlinkShow;
			_parent->UpdateRegion(GetCaretRegion());
		}

	}
	void HideCaret() {
		if (_caret_state != CaretState::Hide) {
			_caret_state = CaretState::Hide;
			_parent->UpdateRegion(region_infinite);
		}
	}

	const Rect GetCaretRegion() const {
		return Rect(_parent->ConvertBaseLayerPointToPoint(_caret_point), caret_size);
	}

public:
	CaretBlinker(ScrollWnd& parent) : _parent(parent), _caret_blink_timer(parent) {}

	bool IsActive() const { return _caret_state != CaretState::Hide; }
	const Point GetPosition() const { return _caret_point; }

	void TrackMsg(Msg msg, Para para) {
		if (IsMouseMsg(msg) || IsKeyboardMsg(msg)) {
			StartBlinkingCaret();
		} else if (msg == Msg::Timer) {
			BlinkCaret();
		} else if (msg == Msg::LoseFocus) {
			HideCaret();
		}

		if (_click_tracker.TrackMsg(msg, para)) {
			SetCaret(BoundPointToNearestLatticePoint(GetMouseMsg(para).point, Size(16, 16)));
		}
	}

	void AppendFigure(FigureQueue& figure_queue) {
		// For drawing caret.
		if (_caret_state == CaretState::Show || _caret_state == CaretState::BlinkShow) {
			figure_queue.append(new ColorFigure(caret_color), GetCaretRegion());
		}
	}
};