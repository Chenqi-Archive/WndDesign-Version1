#include "text_block.h"
#include "main_frame.h"


inline Point BoundPointToNearestLatticePoint(Point point, Size grid_size) {
	int width = static_cast<int>(grid_size.width), height = static_cast<int>(grid_size.height);
	if (width <= 1 || height <= 1) { return point; }

	int remainder_x = point.x % width;
	if (remainder_x < 0) { remainder_x += width; }
	point.x -= remainder_x;
	if (remainder_x > width / 2) { point.x += width; }

	int remainder_y = point.y % height;
	if (remainder_y < 0) { remainder_y += height; }
	point.y -= remainder_y;
	if (remainder_y > height / 2) { point.y += height; }
	return point;
}



inline TextBlock::Style::Style(Point point) {
	size.normal(100px, 100px).min(100px, 0px).max(800px, length_max_tag);
	auto_resize.width_auto_resize().height_auto_resize();
	position.left(px(point.x)).top(px(point.y)); // Minus the border width and the padding.
	border.width(1px).color(ColorSet::Gray);
	background.setColor(color_transparent);
	padding.left(5px).top(12px).down(5px).right(5px);
	paragraph.word_wrap(WordWrap::Character).line_height(120pct);
	font.family(L"µÈÏß").size(19px).color(ColorSet::Black);
}

TextBlock::TextBlock(MainFrame& frame, Point point) :EditBox(nullwnd, Style(point)), _frame(frame), _mouse_drag_tracker(*this) {}

bool TextBlock::Handler(Msg msg, Para para) {
	bool handled = false;
	if (Vector vector = _mouse_drag_tracker.TrackMsg(msg, para); vector != vector_zero) {
		if (_mouse_drag_type == MouseDragType::Moving) {
			Point old_point = (*this)->ConvertToLayerPoint(point_zero);
			Point new_point = BoundPointToNearestLatticePoint(old_point + vector, Size(20, 20));
			if (new_point != old_point) {
				(*this)->SetPosition(new_point.x, new_point.y);
			}
			handled = true;
		} else if (_mouse_drag_type == MouseDragType::Resizing) {
			_mouse_drag_tracker.SetDownPosition(GetMouseMsg(para).point);
			if (vector.x != 0) {
				auto& style = (*this)->GetStyle();
				style.size._normal.width = px(static_cast<int>((*this)->GetSize().width) + vector.x);
				style.auto_resize.width_auto_resize(false);
				(*this)->RegionStyleUpdated();
			}
			handled = true;
		}
	}

	if (bool has_clicked = _mouse_click_tracker.TrackMsg(msg); has_clicked == true) {
		if (_mouse_drag_type == MouseDragType::Moving) {
			// The moving bar has been clicked. Select the whole textbox.
			_block_selected = true;
			(*this)->SetFocus();
			(*this)->UpdateRegion(region_infinite);
			return true;
		} else {
			if (_block_selected == true) {
				_block_selected = false;
				(*this)->UpdateRegion(region_infinite);
			}
		}
	}

	// Drag on top border, and resize on right border.
	if (msg == Msg::LeftDown) {
		Point point = GetMouseMsg(para).point;
		Size size = (*this)->GetSize();
		if (uint x = static_cast<uint>(point.x); x > size.width - 4 && x < size.width) {
			_mouse_drag_type = MouseDragType::Resizing;
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
	}

	if (msg == Msg::KeyDown && _block_selected == true) {
		switch (GetKeyMsg(para).key) {
		case Key::Backspace: 
		case Key::Delete: _frame.DestroyBlock(_it_block); return true;
		default: break;
		}
	}

	if (msg == Msg::LoseFocus) {
		if ((*this)->GetText().empty()) {
			_frame.DestroyBlock(_it_block); return true;
		}
		if (_block_selected == true) {
			_block_selected = false;
			(*this)->UpdateRegion(region_infinite);
		}
	}

	if (!handled) {
		EditBox::Handler(msg, para);
	}

	return true;
}

void TextBlock::AppendFigure(FigureQueue& figure_queue) {
	Size size = (*this)->GetSize();
	if (_block_selected == true) {
		figure_queue.Push(ColorFigure::Create(mask_color), Rect(point_zero, size));
	} else {
		EditBox::AppendFigure(figure_queue);
	}
	figure_queue.Push(ColorFigure::Create(mask_color), Rect(point_zero, Size(size.width, 7)));
	figure_queue.Push(Line::Create(Point(0, 0), Point(30, 0), ColorSet::Gray, 1), Rect((size.width - 30) / 2, 3, 30, 1));
}
