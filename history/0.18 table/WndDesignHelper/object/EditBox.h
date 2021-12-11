#pragma once

#include "WndTypeBase.h"
#include "../style/editbox_style.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


// A simple implementation of editbox based on textbox.
// It supports caret, selection, clipborad, keyboard and IME input.
// It does not support undo/redo until now. And it may not be very efficient for long text.

// Another version will be based on scrollwnd and will enable auto resizing with text
//   and scrolling. And the caret and selection will be drawn as on the top layer, 
//   so the text will not be redrawn every time the caret or selection changes.
class EditBox : public TextBox {
public:
	EditBox(const ObjectBase& parent, const EditBoxStyle& style);

private:
	EditBoxStyle::EditStyle _edit_style;

	// For drawing caret and selection when the window composites.
	void AppendFigure(FigureQueue& figure_queue) override;


	////////////////////////////////////////////////////////////
	////                Main Message Handler                ////
	////////////////////////////////////////////////////////////
private:
	bool _has_mouse_down;
	Point _mouse_down_position;

	bool _has_ctrl_down;

private:
	// Updates the cursor style depending on the mouse position.
	// Set to Cursor::Text when falles on text, and Cursor::Normal on other regions.
	void UpdateCursor(Point mouse_move_position) {
		if ((*this)->HitTestPoint(mouse_move_position).is_inside) {
			SetCursor(Cursor::Text);
		} else {
			SetCursor(Cursor::Default);
		}
	}

	bool OnMouseMsg(Msg msg, Para para);
	bool OnKeyboardMsg(Msg msg, Para para);
protected:
	bool Handler(Msg msg, Para para) override;


	///////////////////////////////////////////////////////////
	////                       Caret                       ////
	///////////////////////////////////////////////////////////
private:
	enum class CaretState { Hide, Show, BlinkHide, BlinkShow} _caret_state;
 
	// Caret blinking.
	static const uint caret_blink_period = 500;   // 500ms
	static const uint caret_blink_expire_time = 20000; // 20s. After 20s without any action. The caret will remain showing.
	Timer _timer; 
	uint _caret_blink_time;

	// Caret positioning.
	static const uint caret_width = 1;
	uint _caret_text_position;		// The caret position of the entire text.
	//uint _caret_position_line;		// The line number caret lies. 
	//uint _caret_position_character; // The character of the line caret lies. (0 for line beginning, -1 for line end.)
	Rect _caret_region;				// The caret region to draw.

private:
	// Hide caret when focus is lost.
	void HideCaret();
	// Start blinking caret.
	void StartBlinkingCaret();
	// Blink caret when timer alerts.
	void BlinkCaret();

private:
	// Helper function for SetCaret() and SelectText().
	// It uses hit test info to set current caret position and region.
	void UpdateCaret(const HitTestInfo& info);

	// Helper function to get the length of the character of current caret position.
	// Used for deleting a character.
	uint GetCharacterLength(uint text_position);

private:
	// For setting the caret when mouse clicks. 
	// Selection begin position will be updated and old selection will be cleared.
	void SetCaret(Point mouse_down_position);

	// For setting the caret when moving caret or inserting new text.
	// If is_trailing_hit is true, the new caret position is at the trailing of 
	//   the character indicated by text_position.
	void SetCaret(uint text_position, bool is_trailing_hit);

	// For moving caret position by keyboard.
	// If caret position is moved, selection will be cleard.
	enum class MoveCaretDirection{Left, Right, Up, Down, Home, End};
	void MoveCaret(MoveCaretDirection direction);


	///////////////////////////////////////////////////////////
	////                     Selection                     ////
	///////////////////////////////////////////////////////////
private:
	uint _mouse_down_text_position;
	uint _selection_begin;
	uint _selection_end;
	vector<HitTestInfo> _selection_info;

	bool HasSelection() const { return _selection_end > _selection_begin; }

	// When mouse has clicked down and moves, select the text. 
	void DoSelection(Point mouse_move_position);

	// Clear selection when set caret or delete selected text.
	void ClearSelection();


	////////////////////////////////////////////////////////////
	////                   Keyboard Input                   ////
	////////////////////////////////////////////////////////////
private:
	// Insert ch at current caret position, or replace selected text.
	void Insert(wchar ch);

	// Insert text_string at current caret position, or replace selected text.
	void Insert(const wstring& str);

	// Delete the selection of text. If no selection, delete the latter character of the caret position,
	//   or delete the former if is_backspace is true.
	void Delete(bool is_backspace);


	/////////////////////////////////////////////////////////////
	////                      IME Input                      ////
	/////////////////////////////////////////////////////////////
private:
	uint _ime_composition_begin;
	uint _ime_composition_end;

	// Remember current composition begin position as the caret position.
	// And set the composition string length to 0;
	void ImeStartComposition();

	// If there's selection, delete the selected text.
	// Replace composition string with the new composition string(or result string), reset composition length,
	//   and move caret to the end of the string.
	// It doesn't matter whether the string is result string or not.
	// If it is result string, the composition begin will be reset when next ImeStartComposition is called.
	void ImeComposition(const wstring& composition_string);


	/////////////////////////////////////////////////////////////
	////                      Clipboard                      ////
	/////////////////////////////////////////////////////////////
private:
	// Cut the current text selection to clipboard.
	void Cut();
	// Copy the current text selection to clipboard.
	void Copy();
	// Replace the current selection from clipboard.
	void Paste();
};


END_NAMESPACE(WndDesignHelper)