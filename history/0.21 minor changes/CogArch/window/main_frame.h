#pragma once

#include "menu_bar.h"
#include "space_view.h"
#include "clipboard_view.h"

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
	void EnterParentView(const vector<Block*>& blocks);


	////////////////////////////////////////////////////////////
	////                 Clipboard Managing                 ////
	////////////////////////////////////////////////////////////
private:
	ClipboardView _clipboard_view;
	//unique_ptr<TextBox> _dragging_item;
	ClipboardItem* _dragging_item = nullptr;

	void LoadClipboard() {
		for (auto it : _block_manager.Clipboard()) {
			_clipboard_view.AddItem(*it);
		}
	}

public:
	void BlockRemoved(Block& block) {
		if (!_block_manager.TryDestroyEmptyBlock(block)) {
			if (_block_manager.AddBlockToClipboard(block)) {
				_clipboard_view.AddItem(block);
			}
		}
	}

public:
	void DragItem(ClipboardItem& item) {
		_dragging_item = &item;
		_space_view->GetWnd()->SetCapture();
	}

	bool IsDraggingItem() const { return _dragging_item != nullptr; }

	void CancleDragging() { _dragging_item = nullptr; }

	Block& DropItem() { 
		if (_dragging_item == nullptr) { throw; }
		auto& block = _dragging_item->_block; 
		_clipboard_view.DropItem(*_dragging_item);
		_block_manager.RemoveBlockFromClipboard(block);
		_dragging_item = nullptr; 
		return block; 
	}
};