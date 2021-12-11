#pragma once

#include "block_view.h"
#include "space_description.h"


class MainFrame;

class SpaceView : public ScrollWnd {
private:
	MainFrame& _main_frame;

public:
	SpaceView(MainFrame& main_frame, const vector<Block*>& blocks, bool is_parent_view);


	////////////////////////////////////////////////////////////
	////                Child Block Managing                ////
	////////////////////////////////////////////////////////////
private:
	const vector<Block*> _current_blocks;  // current space blocks.
	SpaceDescription _space_description;
	list<BlockView> _child_blocks;  // parent blocks if is parent view.
	bool _is_parent_view;  // In abstract mode.

public:
	BlockView& AddBlock(Block& block, Point position, uint width, bool create_relation);
	void RemoveBlock(BlockView& block);
	void RemoveSelectedBlocks();

	// For space switching.
private:
	void EnterMyParentView();
public:
	void EnterChildView(Block& block);
	void EnterParentView(Block& block);
	void EnterSelectedBlocksParentView();

private:
	inline static Color selection_color = Color(ColorSet::Gray, 0x7F);
	vector<BlockView*> _selected_blocks;

public:
	void UndoSelection() { 
		for (auto it : _selected_blocks) { it->Unselect(); }
		_selected_blocks.clear();
	}
	void SelectMoreBlocks(Rect region_on_layer) {
		for (auto& child : _child_blocks) {
			if (child->GetRegionOnLayer().Intersect(region_on_layer).IsEmpty()) { continue; }
			if (child.Select()) { _selected_blocks.push_back(&child); }
		}
		(*this)->SetFocus();
	}
	void SelectMoreBlocks(BlockView& block_view) {
		if (block_view.Select()) { _selected_blocks.push_back(&block_view); }
		(*this)->SetFocus();
	}

private:
	Rect _selection_region = region_empty;  // relative to window
private:
	void UpdateSelectionRegion(Rect selection_region) {
		if (_selection_region != selection_region) {
			Rect union_region = selection_region.Union(_selection_region);
			_selection_region = selection_region;
			(*this)->UpdateRegion(union_region);
		}
	}


	///////////////////////////////////////////////////////////
	////                  Message Handling                 ////
	///////////////////////////////////////////////////////////
private:
	enum class MouseDragType { None, Dragging, Selecting } _mouse_drag_type = MouseDragType::None;
	Point _mouse_down_position = point_zero;
	bool _has_mouse_dragged = false;


	// For drawing caret.
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

	void SetCaret(Point point);
	void StartBlinkingCaret();
	void BlinkCaret();
	void HideCaret();

	const Rect GetCaretRegion() const {
		return Rect(ConvertBaseLayerPointToPoint(_caret_point), caret_size);
	}


private:
	const Point ConvertPointToBaseLayerPoint(Point point) const {
		return point + ((*this)->GetScrollPosition() - point_zero);
	}
	const Point ConvertBaseLayerPointToPoint(Point point) const {
		return point - ((*this)->GetScrollPosition() - point_zero);
	}

private:
	bool Handler(Msg msg, Para para);

	void OnMouseMsg(Msg msg, Para para);
	void OnKeyboardMsg(Msg msg, Para para);

	// For drawing caret.
	void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) override;
};