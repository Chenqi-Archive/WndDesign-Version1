#pragma once

#include "WndTypeBase.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


struct EditBoxStyle : public TextBoxStyle {
	// The possible actions with textbox.
	struct EditStyle {
	public:
		Color _selection_color = Color(ColorSet::DimGray, 0x7f);
		Color _caret_color = ColorSet::DimGray;
		bool _disable_select = false;
		bool _disable_caret = false;
		bool _disable_copy = false;
		bool _disable_edit = false;
	public:
		constexpr EditStyle& selection_color(Color selection_color) { _selection_color = selection_color; return *this; }
		constexpr EditStyle& caret_color(Color caret_color) { _caret_color = caret_color; return *this; }
		constexpr EditStyle& disable_select() { _disable_select = true; return *this; }
		constexpr EditStyle& disable_caret() { _disable_caret = true; return *this; }
		constexpr EditStyle& disable_copy() { _disable_copy = true; return *this; }
		constexpr EditStyle& disable_edit() { _disable_edit = true; return *this; }
	}edit;
};


// A simple implementation of editbox based on textbox.
// It supports caret, selection, clipboard and input method.
// But it may not be very efficient for long text, and it does not support undo/redo.

// Another version will be based on scrollwnd and will enable auto resizing with text
//   and scrolling. And the caret and selection will be drawn as on the top layer, 
//   so the text will not be redrawn every time the caret or selection changes.
class EditBox : public TextBox {
public:
	EditBox(const ObjectBase& parent, const EditBoxStyle& style) :
		TextBox(parent, style),
		_edit_style(style.edit),
		_has_mouse_down(false),
		_mouse_down_position(point_zero),

		_caret_state(CaretState::Hide),
		_caret_region(region_empty),

		_selection_begin(0),
		_selection_end(0),
		_selection_info(),

		_ime_composition_begin(0),
		_ime_composition_end(0) {
	}

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
	bool Handler(Msg msg, Para para) override;


	///////////////////////////////////////////////////////////
	////                       Caret                       ////
	///////////////////////////////////////////////////////////
private:
	static const uint caret_width = 1;

	enum class CaretState { Hide, Show } _caret_state;

	uint _caret_text_position;		// The caret position of the entire text.
	//uint _caret_position_line;		// The line number caret lies. 
	//uint _caret_position_character; // The character of the line caret lies. (0 for line beginning, -1 for line end.)
	Rect _caret_region;				// The caret region to draw.

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

	// Hide caret when focus is lost.
	void HideCaret();

	// Show caret when focus is on.
	void ShowCaret();

	// For moving caret position by keyboard.
	// If caret position is moved, selection will be cleard.
	enum class MoveCaretDirection{Left, Right, Up, Down, Home, End};
	void MoveCaret(MoveCaretDirection direction);


	///////////////////////////////////////////////////////////
	////                     Selection                     ////
	///////////////////////////////////////////////////////////
private:
	uint _selection_begin;
	uint _selection_end;
	vector<HitTestInfo> _selection_info;

	bool HasSelection() const { return _selection_end > _selection_begin; }

	// When mouse has clicked down and moves, select the text. 
	void DoSelection(Point mouse_move_position);


	////////////////////////////////////////////////////////////
	////                   Keyboard Input                   ////
	////////////////////////////////////////////////////////////
private:
	// Insert ch at current caret position, or replace selected text.
	void Insert(wchar ch);

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
};


END_NAMESPACE(WndDesignHelper)