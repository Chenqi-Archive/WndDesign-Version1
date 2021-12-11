#pragma once

#include "WndDesign.h"

#include "menu_bar.h"
#include "text_block.h"

#include <list>

using std::list;


class MainFrame :public ScrollWnd {
	//////////////////////////////////////////////////////////
	////       Initialization and Messages Handling       ////
	//////////////////////////////////////////////////////////
	struct Style : ScrollWndStyle { Style(); };

public:
	MainFrame();
	MainFrame(const wstring& file);
	bool Handler(Msg msg, Para para);

	// For mouse messages.
private:
	bool _has_mouse_down = false;
	Point _mouse_down_position = point_zero;
	bool _has_mouse_dragged = false;

	WindowBorderResizer _border_resizer;

private:
	void OnMouseMsg(Msg msg, Para para);
	void OnKeyboardMsg(Msg msg, Para para);


	// For drawing caret.
	void AppendFigure(FigureQueue& figure_queue) override;


	// Menu bar.
private:
	MenuBar _menu_bar;


	//////////////////////////////////////////////////////////
	////                  Block Managing                  ////
	//////////////////////////////////////////////////////////
	//// For displaying a temporary caret when mouse clicks.
private:
	enum class CaretState { Hide, Show, BlinkHide, BlinkShow } _caret_state = CaretState::Hide;

	inline static const Color caret_color = ColorSet::Gray;

	// Caret positioning.
	inline static const Size caret_size = Size(1, 20); // (caret height may depending on the font size)
	Point _caret_point = point_zero;		// The caret point to draw. (relative to base layer)

	const Rect GetCaretRegion() const {
		return Rect(_caret_point - ((*this)->GetScrollPosition() - point_zero), caret_size);
	}

	// Caret blinking.
	inline static const uint caret_blink_period = 500;   // 500ms
	inline static const uint caret_blink_expire_time = 20000; // 20s. After 20s without any action. The caret will remain showing.
	Timer _caret_blink_timer;
	uint _caret_blink_time = 0;

private:
	void SetCaret(Point mouse_click_point);// Set caret when mouse clicks. (point is relative to main_frame)
	void StartBlinkingCaret();	// Start blinking caret when there is user action.
	void BlinkCaret();	// Blink caret when timer alerts.
	void HideCaret();	// Hide caret when focus loses.


	//// For managing blocks.
private:
	// May use spatial index to manage blocks.
	list<TextBlock> _blocks;

public:
	void DestroyBlock(list<TextBlock>::iterator it_block) { _blocks.erase(it_block); }


	///////////////////////////////////////////////////////////
	////                  File Operations                  ////
	///////////////////////////////////////////////////////////
private:
	// Open the file, if already open a file, save the old file.
	void Load(const wstring& file);
	// Save to file, if no file opened, display a dialog to select file.
	void Save();
};