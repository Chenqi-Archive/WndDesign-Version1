#include "block_view.h"
#include "space_view.h"
#include "helper.h"


struct BlockViewStyle : public EditBoxStyle {
    BlockViewStyle(Point point, ValueTag width) {
		size.normal(width, 100px).min(100px, 0px).max(800px, length_max_tag);
		if (width.IsAuto()) { auto_resize.width_auto_resize();}
		auto_resize.height_auto_resize();
		position.left(px(point.x)).top(px(point.y));
		border.width(1px).color(ColorSet::Gray);
		background.setColor(color_transparent);
		padding.left(5px).top(12px).down(5px).right(5px);
		paragraph.word_wrap(WordWrap::Character).line_height(120pct);
		font.family(L"µÈÏß").size(20px).color(ColorSet::Black);
	}
};


BlockView::BlockView(SpaceView& space, Block& block, Point position, uint width) :
	EditBox(nullwnd, BlockViewStyle(position, width == child_width_auto ? length_auto : px(width)), block.Content()),
	_space(space),
	_block(block),
	_mouse_click_tracker(),
	_mouse_drag_tracker(*this) {
	space->AddChildStatic(GetWnd());
}


bool BlockView::Handler(Msg msg, Para para) {
	bool handled = false;
	if (Vector vector = _mouse_drag_tracker.TrackMsg(msg, para); vector != vector_zero) {
		if (_mouse_drag_type == MouseDragType::Moving) {
			Point old_point = (*this)->ConvertToLayerPoint(point_zero);
			Point new_point = BoundPointToNearestLatticePoint(old_point + vector, Size(16, 16));
			if (new_point != old_point) {
				if (_is_selected) {
					_space.MoveSelectedBlocks(new_point - old_point);
				} else {
					(*this)->SetPosition(new_point.x, new_point.y);
					_space.SetChildPosition(_block, new_point);
				}
			}
			handled = true;
		} else if (_mouse_drag_type == MouseDragType::Resizing) {
			int new_width = BoundNumberToNearestLatticeNumber(static_cast<int>(_mouse_down_width) + vector.x, 16);
			auto& style = (*this)->GetStyle();
			if (style.size._normal.width.AsSigned() != new_width) {
				style.size._normal.width = px(new_width);
				style.auto_resize.width_auto_resize(false);
				(*this)->RegionStyleUpdated();
				_space.SetChildWidth(_block, new_width);
			}
			handled = true;
		}
	}

	if (bool has_clicked = _mouse_click_tracker.TrackMsg(msg); has_clicked == true) {
		if (_mouse_drag_type == MouseDragType::Moving) {
			// The moving bar has been clicked. Select the whole textbox.
			if (GetMouseMsg(para).ctrl != true) {
				(*this)->SetFocus(); // This will cause space view unselect other blocks.
			}
			_space.SelectMoreBlocks(*this);
			return true;
		} else {
			(*this)->SetFocus();
		}
	}

	// Drag on top border, and resize on right border.
	if (msg == Msg::LeftDown) {
		Point point = GetMouseMsg(para).point;
		Size size = (*this)->GetSize();
		if (uint x = static_cast<uint>(point.x); x > size.width - 4 && x < size.width) {
			_mouse_drag_type = MouseDragType::Resizing;
			_mouse_down_width = size.width;
			handled = true;
		} else if (uint y = static_cast<uint>(point.y); y <= 6) {
			_mouse_drag_type = MouseDragType::Moving;
			handled = true;
		} else {
			_mouse_drag_type = MouseDragType::None;
		}
	} else if (msg == Msg::MouseMove) {
		Point point = GetMouseMsg(para).point;
		Size size = (*this)->GetSize();
		if (uint x = static_cast<uint>(point.x); x > size.width - 4 && x < size.width) {
			SetCursor(Cursor::Resize0);
			handled = true;
		} else if (uint y = static_cast<uint>(point.y); y <= 6) {
			SetCursor(Cursor::Move);
			handled = true;
		}
	} else if (msg == Msg::MouseWheel) {
		auto& mouse_msg = GetMouseMsg(para);
		if (mouse_msg.wheel_delta > 0) {
			_space.EnterChildView(_block);
			return true;
		} else {
			if (mouse_msg.ctrl == true && mouse_msg.shift == true) {
				_space.EnterParentView(_block);
			} else {
				//if (Block* current_parent = _block.GetCurrentParent(); current_parent != nullptr) {
				//	_space.EnterChildView(*current_parent);
				//}
				return true;
			}
		}
	}

	if (!handled) {
		EditBox::Handler(msg, para);
	}

    return true;
}

void BlockView::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) {
	Size size = (*this)->GetSize();
	figure_queue.append(new ColorFigure(mask_color), Rect(point_zero, Size(size.width, 7)));
	figure_queue.append(new Line(Point(0, 0), Point(30, 0), ColorSet::Gray, 1), Rect((size.width - 30) / 2, 3, 30, 1));
	if (_is_selected == true) {
		figure_queue.append(new ColorFigure(mask_color), region_to_update);
	} else {
		EditBox::AppendFigure(figure_queue, region_to_update);
	}
}
