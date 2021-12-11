#define ObjectTest

#ifdef ObjectTest

#include "../WndDesignHelper/WndDesignHelper.h"

#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")


using namespace WndDesignHelper;

class MainFrame;

class MyEditBox : public EditBox {
	struct Style : EditBoxStyle {
		Style() {
			size.max(100pct, 100pct);
			position.left(0px).top(0px);
			background.setColor(color_transparent);
			border.color(ColorSet::Gray).width(3px);
			padding.setAll(10px);
			paragraph.word_wrap(WordWrap::Character);
			font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
			auto_resize.set(true, true);
		}
	};

private:
	MainFrame& _parent;
	MouseDragTracker _mouse_drag_tracker;

public:
	MyEditBox(MainFrame& parent);
	bool Handler(Msg msg, Para para) override {
		if (EditBox::Handler(msg, para)) { return true; }

		Vector moving_vector = _mouse_drag_tracker.TrackMsg(msg, para);
		if (moving_vector != vector_zero) {
			Point origin_point = (*this)->ConvertToLayerPoint(point_zero);
			Point new_point = origin_point + moving_vector;
			(*this)->SetPosition(px(new_point.x), px(new_point.y));
			return true;
		}
		return false;
	}
};

class MainFrame : public SimpleScrollWnd {
private:
	struct Style : ScrollWndStyle {
		Style() {
			size.normal(800px, 480px);
			position.setHorizontalCenter().setVerticalCenter();
			background.setColor(ColorSet::White);
			border.width(3px).color(ColorSet::Black);
			entire_region.Set(0pct, 0pct, 100pct, 300pct); // = region_infinite_tag;
		}
	};
private:
	MouseDragTracker _mouse_drag_tracker;
	MyEditBox _textbox;

public:
	MainFrame() :SimpleScrollWnd(desktop, Style()), _mouse_drag_tracker(*this), _textbox(*this) {}
	bool Handler(Msg msg, Para para) override {
		if (SimpleScrollWnd::Handler(msg, para)) { return true;}
		Vector moving_vector = _mouse_drag_tracker.TrackMsg(msg, para);
		if (moving_vector != vector_zero) {
			(*this)->MoveView(Vector(-moving_vector.x, -moving_vector.y));
			_mouse_drag_tracker.SetDownPosition(GetMouseMsg(para).point);
		}
		if (msg == Msg::RightUp) {
			desktop->Destroy();
			return true;
		}
		return true;
	}
};


MainFrame* main_frame;

void Init() {
	main_frame = new MainFrame();
}

void Final() {
	delete main_frame;
}

MyEditBox::MyEditBox(MainFrame& parent) : EditBox(nullwnd, Style()), _parent(parent), _mouse_drag_tracker(*this) {
	const wchar text[] = L"请在此输入文本：";

	(*this)->SetText(text);
	_parent->AddChildStatic(GetWnd());
}


#endif