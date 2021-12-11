#pragma once

#include "../WndDesign.h"

using 
WndDesign::Uncopyable,
WndDesign::Point, 
WndDesign::uint;


#include <string>
#include <vector>
#include <list>

using std::wstring;  // For storing block contents.
using std::vector;   // For maintaining parent and child blocks.  (May use list or set later.)
using std::list;

#include <algorithm>


class Block;


struct _Child_Block_Info {
	Block* child;
	Point child_position;
	uint child_width;  // height will auto resize
};

struct _Parent_Block_Info {
	Block* parent;
	list<_Child_Block_Info>::iterator iterator;
	operator Block* () const { return parent; }
};

inline static constexpr Point child_position_auto = Point(0x7FFFFFFF, 0x7FFFFFFF);
inline static constexpr uint child_width_auto = 0xFFFFFFFF;


class Block : Uncopyable {
private:
	friend class BlockManager;
	list<Block>::iterator _iterator;  // For quick locating my position in block manager.
	bool _is_in_clipboard = false;

public:
	mutable uint _temporary_data = 0;  // temporapy value used when saving to file

private:
	wstring _content;
	Point _space_center_position;

	vector<_Parent_Block_Info> _parent_blocks;
	Block* _current_parent_block = nullptr;  // The current parent block.

	list<_Child_Block_Info> _child_blocks;


	//vector<Block*> dependent_blocks;
	//vector<Block*> referenced_blocks;

	vector<_Parent_Block_Info>::iterator FindParentBlock(Block* parent) {
		return std::find(_parent_blocks.begin(), _parent_blocks.end(), parent);
	}


public:
	auto& Content() { return _content; }
	const auto& Content() const { return _content; }
	const auto& ChildBlocks() const { return _child_blocks; }
	const auto& ParentBlocks() const { return _parent_blocks; }

	const auto& GetCurrentParent() const { return _current_parent_block; }

	void SetSpaceCenterPosition(Point position) {
		_space_center_position = position;
	}

	// Current parent is modifyed every time the block is added to a SpaceView as a BlockView.
	void SetCurrentParent(Block* parent) {
		if (auto it_parent = FindParentBlock(parent); it_parent != _parent_blocks.end()) {
			_current_parent_block = parent;
		}
	}

	void SetPosition(Block& parent, Point position) {
		if (auto it_parent = FindParentBlock(&parent); it_parent != _parent_blocks.end()) {
			it_parent->iterator->child_position = position;
		}
	}
	void SetWidth(Block& parent, uint width) {
		if (auto it_parent = FindParentBlock(&parent); it_parent != _parent_blocks.end()) {
			it_parent->iterator->child_width = width;
		}
	}
};