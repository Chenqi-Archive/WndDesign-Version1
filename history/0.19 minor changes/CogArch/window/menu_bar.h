#pragma once

#include "WndDesign.h"


class MainFrame;


class MenuBar : public Wnd{
	struct Style : WndStyle { Style(); };

private:
	MainFrame& _main_frame;

private:
	class Title : public TextBox {
		struct Style :TextBoxStyle { Style(); };
	public:
		Title(MenuBar& menu_bar) :TextBox(menu_bar, Style()) {}
		bool Handler(Msg msg, Para para) { return false; } // The message will be handled by menu bar.
	} _title;

	class CloseButton : public Button {
		struct Style :ButtonStyle { Style(); };
		MenuBar& _menu_bar;
	public:
		CloseButton(MenuBar& menu_bar) : Button(menu_bar, Style()), _menu_bar(menu_bar) {}
		void OnClick() override;
		void AppendFigure(FigureQueue& figure_queue) override; // For drawing "x"
	} _close_button;

	class Menu : public ButtonWithText {
		struct Style : ButtonStyle { Style(uint start_position); };
	public:
		Menu(MenuBar& menu_bar, uint start_position, const wstring& text);
	} _file_menu;


private:
	SystemWndDragTracker _system_wnd_drag_tracker;  // For moving main_frame when mouse drags.

private:
	FadingAnimation _fading_animation;

public:
	MenuBar(MainFrame& main_frame, const wstring& title);
	bool Handler(Msg msg, Para para);

public:
	void SetTitle(const wstring& title) { _title->SetText(title); }
};