#pragma once

#include "WndDesign.h"


class MainFrame;

class MenuBar : public Wnd {
private:
	class Title : public TextBox {
	private:
		wstring _title;
	public:
		Title(MenuBar& menu_bar);
		void Set(const wstring& title) { _title = title; (*this)->TextUpdated(); }
	};

	class CloseButton : public Button {
		MainFrame& _main_frame;
	public:
		CloseButton(MenuBar& menu_bar);
		void OnClick() override;
		void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) override; // For drawing "x"
	};

private:
	MainFrame& _main_frame;
	Title _title;
	CloseButton _close_button;

private:
	SystemWindowDragTracker _drag_tracker;  // For moving main_frame when mouse drags.
	
	// Auto hide the window when mouse leaves, and show it when mouse hovers.
	class ShowUpAnimation {
	private:
		ObjectBase& _parent;
	public:
		ShowUpAnimation(ObjectBase& parent) :_parent(parent) {}
		void TrackMsg(Msg msg, Para para) {
			if (IsMouseMsg(msg)) {
				if (_parent->GetStyle().render._opacity != 0xFF) {_parent->SetOpacity(0xFF);}
			}
			if (msg == Msg::MouseLeave) {
				if (_parent->GetStyle().render._opacity != 0x00) { _parent->SetOpacity(0x00); }
			}
		}
	}_show_up_animation;

	bool Handler(Msg msg, Para para);

public:
	MenuBar(MainFrame& main_frame);
	void SetTitle(const wstring& title) { _title.Set(title); }
};
