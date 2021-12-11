#pragma once

#include "menu_bar.h"
#include "space_view.h"

#include "../file/cga_file.h"
#include "../block/block_manager.h"

#include <memory>

using std::unique_ptr;


class MainFrame : public Wnd {
public:
	MainFrame(const wstring& file);
	~MainFrame();

private:
	MenuBar _menu_bar;

private:
	SystemWindowBorderResizer _border_resizer;
	bool Handler(Msg msg, Para para);


	///////////////////////////////////////////////////////////
	////                   File Managing                   ////
	///////////////////////////////////////////////////////////
private:
	CGAFormat::CGAFile _file;
public:
	BlockManager _block_manager;


	////////////////////////////////////////////////////////////
	////                   View Switching                   ////
	////////////////////////////////////////////////////////////
private:
	unique_ptr<SpaceView> _space_view;

public:
	void EnterChildView(Block* block);
	void EnterParentView(const vector<Block*> blocks);
};