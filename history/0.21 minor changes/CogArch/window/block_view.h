#pragma once

#include "WndDesign.h"

#include "../block/block.h"

#include <list>

using std::list;


class SpaceView;
class SpaceDescription;

class BlockView : public EditBox {
private:
	SpaceView& _space;

public:
	list<BlockView>::iterator _iterator; // Managed by SpaceView.

private:
	MouseDragTracker _mouse_drag_tracker;
	MouseClickTracker _mouse_click_tracker;
	enum class MouseDragType { None, Moving, Resizing } _mouse_drag_type = MouseDragType::None;
	uint _mouse_down_width = 0;

	inline static const Color mask_color = Color(ColorSet::LightGray, 0x7F);

	bool _is_selected = false;
public:
	bool Select() {  // Returns false if the block has already been selected.
		if (!_is_selected) {
			_is_selected = true;
			(*this)->UpdateRegion(region_infinite);
			return true;
		}
		return false;
	}
	void Unselect() {
		if (_is_selected) {
			_is_selected = false;
			(*this)->UpdateRegion(region_infinite);
		}
	}


public:
	Block& _block;
	
public:
	BlockView(SpaceView& space, Block& block, Point position, uint width);

public:
	bool Handler(Msg msg, Para para);
	void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) override;	// For drawing moving bar and mask.
};