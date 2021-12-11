#pragma once

#include "WndDesign.h"

#include <list>

using std::list;


class MainFrame;  // The scrollable main frame declaration.


// Using Editbox as TextBlock.
// May use Table to contain multiple paragraphs for later versions.
class TextBlock : public EditBox {
	struct Style : public EditBoxStyle { Style(Point point); };

private:
	MouseDragTracker _mouse_drag_tracker;
	enum class MouseDragType{None, Moving, Resizing} _mouse_drag_type = MouseDragType::None;

	inline static const Color mask_color = Color(ColorSet::LightGray, 0x7F);
	MouseClickTracker _mouse_click_tracker;
	bool _block_selected = false;

private:
	MainFrame& _frame;
	list<TextBlock>::iterator _it_block;

public:
	void SetIterator(list<TextBlock>::iterator it_block) { _it_block = it_block; }

public:
	TextBlock(MainFrame& frame, Point point);
	bool Handler(Msg msg, Para para);

	// For drawing moving bar and mask.
	void AppendFigure(FigureQueue& figure_queue) override;

};