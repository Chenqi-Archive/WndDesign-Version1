#include "main_frame.h"


struct MainFrameStyle : WndStyle {
	MainFrameStyle() {
		size.normal(800px, 480px).min(300px, 200px).max(100pct, 100pct);
		position.setHorizontalCenter().setVerticalCenter();
		border.width(5px).color(color_transparent);
	}
};

MainFrame::MainFrame(const wstring& file) :
	Wnd(nullwnd, MainFrameStyle()),
	_menu_bar(*this),
	_clipboard_view(*this),
	_border_resizer(*this) {

	(*this)->ForceBaseLayerRedirected();

	if (!_file.Open(file)) { throw; }
	_menu_bar.SetTitle(file);
	_file.LoadBlockManager(_block_manager);

	LoadClipboard();

	if (_block_manager.Blocks().empty()) {
		_block_manager.CreateBlock().Content().assign(L"Root");
	}

	EnterChildView(&_block_manager.Blocks().front());

	desktop->AddChild(GetWnd());
}

MainFrame::~MainFrame() {
	_file.SaveBlockManager(_block_manager);
}



bool MainFrame::Handler(Msg msg, Para para) {
	if (msg == Msg::ChildHit) {
		msg = GetMouseMsgChildHit(para).original_msg;
	}
	if (_border_resizer.TrackMsg(msg, para) == true) {
		return false;  // Do not send the message to child window.
	}

	return true;
}

void MainFrame::EnterChildView(Block* block) {
	_space_view.reset();
	_space_view.reset(new SpaceView(*this, { block }, false));
}

void MainFrame::EnterParentView(const vector<Block*>& blocks) {
	_space_view.reset();
	_space_view.reset(new SpaceView(*this, blocks, true));
}