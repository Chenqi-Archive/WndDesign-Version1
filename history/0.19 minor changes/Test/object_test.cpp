#define ObjectTest

#ifdef ObjectTest

#include "../WndDesignHelper/WndDesignHelper.h"

#ifdef _DEBUG
#pragma comment(lib, "../build/x64/Debug/WndDesignHelper.lib")
#else
#pragma comment(lib, "../build/x64/Release/WndDesignHelper.lib")
#endif

using namespace WndDesignHelper;

class MyEditBox : public EditBox {
	struct Style : EditBoxStyle {
		Style() {
			position.left(0px).top(0px);
			background.setColor(color_transparent);
			padding.setAll(3px);
			paragraph.word_wrap(WordWrap::Character);
			font.family(L"等线").size(20px).color(ColorSet::CadetBlue);
			auto_resize.width_auto_resize().height_auto_resize();
		}
	};

public:
	MyEditBox() : EditBox(nullwnd, Style()) {
		const wchar text[] = L"请在此输入文本：";
		(*this)->SetText(text);
		// EditBox is added as child by Table.
	}
};


class MyTable :public Table {
	struct Style :TableStyle {
		Style() {
			size.normal(800px, 480px);
			auto_resize.width_auto_resize().height_auto_resize();
			position.setHorizontalCenter().setVerticalCenter();
			background.setColor(ColorSet::White);
			border.width(5px).color(ColorSet::Crimson);
			grid_size.min(20px, 20px);
		}
	};

private:
	MyEditBox _editbox1, _editbox2, _editbox3;
	WindowBorderResizer _resizer;

public:
	MyTable() : Table(nullwnd, Style()), _editbox1(), _editbox2(), _editbox3(), _resizer(*this) {
		(*this)->SetTableSize(TableSize(4, 3));
		(*this)->SetChild(_editbox1.GetWnd(), TablePosition(0, 0));
		(*this)->SetChild(_editbox2.GetWnd(), TablePosition(0, 1));
		(*this)->SetChild(_editbox3.GetWnd(), TablePosition(1, 1));
		desktop->AddChild(GetWnd());
	}
	bool Handler(Msg msg, Para para) override {
		_resizer.TrackMsg(msg, para);
		return Table::Handler(msg, para);
	}
};


MyTable* table;

void Init() {
	table = new MyTable;
}

void Final() {
}

#endif