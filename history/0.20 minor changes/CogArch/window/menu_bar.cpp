#include "menu_bar.h"
#include "main_frame.h"

struct MenuBarStyle : public WndStyle {
    MenuBarStyle() {
        size.setFixed(100pct, 30px);
        position.left(0px).top(0px);
        background.setColor(ColorSet::DarkSlateBlue);
        render.z_index(0xFF);
    }
};

struct TitleStyle : public TextBoxStyle {
    TitleStyle() {
        size.normal(length_auto, 100pct).max(100pct, 100pct);
        auto_resize.width_auto_resize();
        position.down(0px).setHorizontalCenter();
        background.setColor(color_transparent);
        padding.setAll(5px);
        paragraph.text_align(TextAlign::Leading).word_wrap(WordWrap::NoWrap);
        font.size(16px).color(ColorSet::White);
    }
};

struct CloseButtonStyle : public ButtonStyle {
    CloseButtonStyle() {
        size.setFixed(40px, 30px);
        position.right(0px).top(0px);
        background.setColor(color_transparent);
        background_hover.setColor(Color(ColorSet::DarkGray, 0x7F));
        background_down.setColor(Color(ColorSet::Gray, 0x7F));
    }
};


MenuBar::Title::Title(MenuBar& menu_bar) :
    TextBox(menu_bar, TitleStyle(), _title) {
}

MenuBar::CloseButton::CloseButton(MenuBar& menu_bar) :
    Button(menu_bar, CloseButtonStyle()), _main_frame(menu_bar._main_frame) {
}

void MenuBar::CloseButton::OnClick() {
    _main_frame.Destroy();
}

void MenuBar::CloseButton::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) {
    figure_queue.append(new Line(Point(0, 0), Point(10, 10), ColorSet::White, 1), Rect(15, 10, 10, 10));
    figure_queue.append(new Line(Point(10, 0), Point(0, 10), ColorSet::White, 1), Rect(15, 10, 10, 10));
}

MenuBar::MenuBar(MainFrame& main_frame) :
    Wnd(main_frame, MenuBarStyle()),
    _main_frame(main_frame),
    _title(*this),
    _close_button(*this),
    _drag_tracker(*this),
    _show_up_animation(*this) {
}

bool MenuBar::Handler(Msg msg, Para para) {
    bool ret = true;

    if (msg == Msg::ChildHit) {
        const auto& msg_child_hit = GetMouseMsgChildHit(para);

        // If mouse hovers on child window, set the cursor.
        if (msg_child_hit.original_msg == Msg::MouseMove) {
            msg = msg_child_hit.original_msg;  // 
        }

        // Do not send any message to the title textbox.
        if (msg_child_hit.child == _title.GetWnd()) {
            msg = msg_child_hit.original_msg;
            ret = false;
        }
    }

    if (msg == Msg::MouseMove) {
        SetCursor(Cursor::Default);
    }

    _show_up_animation.TrackMsg(msg, para);

    _drag_tracker.TrackMsg(msg, para);

    return ret;
}
