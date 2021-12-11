#pragma once

#include "block_view.h"
#include "space_description.h"

#include "widget/caret_blinker.h"
#include "widget/mouse_drag_selection_tracker.h"
#include "widget/mouse_drag_view_scroller_2.h"


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
	// For block managing.
	BlockView& AddBlock(Block& block, Point position, uint width, bool create_relation);
	void RemoveBlock(BlockView& block);
	void RemoveSelectedBlocks();

	void SetChildPosition(Block& child, Point position) {
		if (!_is_parent_view) {
			if (_current_blocks.size() != 1) { throw; }
			child.SetPosition(*_current_blocks[0], position);
		}
	}
	void SetChildWidth(Block& child, uint width) {
		if (!_is_parent_view) {
			if (_current_blocks.size() != 1) { throw; }
			child.SetWidth(*_current_blocks[0], width);
		}
	}


	// For space switching.
private:
	void EnterMyParentView(bool parent_space);
public:
	void EnterChildView(Block& block);
	void EnterParentView(Block& block);
	void EnterSelectedBlocksParentView(bool parent_space);


private:
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

	void MoveSelectedBlocks(Vector vector);


	///////////////////////////////////////////////////////////
	////                  Message Handling                 ////
	///////////////////////////////////////////////////////////
private:
	CaretBlinker _caret;
	MouseDragSelectionTracker _selection;
	MouseDragViewScroller _scroller;
	CtrlTracker _ctrl_tracker;

	Rect _dragging_item_region = region_empty;

	void UpdateDraggingPosition(Rect region) {
		(*this)->UpdateRegion(_dragging_item_region);
		_dragging_item_region = region;
		(*this)->UpdateRegion(_dragging_item_region);
	}

private:
	bool Handler(Msg msg, Para para);

	// For drawing caret.
	void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) override;
};