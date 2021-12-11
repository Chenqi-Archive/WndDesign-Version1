#pragma once

#include "block.h"



class BlockManager : Uncopyable{
private:
	list<Block> _blocks;

public:
	void Clear() { _blocks.clear(); }
	auto& Blocks() { return _blocks; }
	const auto& Blocks() const { return _blocks; }

	Block& CreateBlock() {
		auto& block = _blocks.emplace_back();
		block._iterator = --_blocks.end();
		return block;
	}

	// If the block has no content or relations, destroy it and return true.
	bool TryDestroyEmptyBlock(Block& block) {
		if (block.Content().empty() && block.ChildBlocks().empty() && block.ParentBlocks().empty()) {
			_blocks.erase(block._iterator);
			return true;
		}
		return false;
	}

private:
	// Clipboard managing.
	vector<Block*> _clipboard;
public:
	const auto& Clipboard() const { return _clipboard; }
	bool AddBlockToClipboard(Block& block) {
		if (!block._is_in_clipboard) {
			_clipboard.push_back(&block);
			block._is_in_clipboard = true;
			return true;
		}
		return false;
	}
	void RemoveBlockFromClipboard(Block& block) {
		if (block._is_in_clipboard) {
			_clipboard.erase(std::find(_clipboard.begin(), _clipboard.end(), &block));
			block._is_in_clipboard = false;
		}
	}

public:
	// Abstract relation.
	void CreateAbstractRelation(Block& parent, Block& child, Point child_position, uint child_width) {
		if (auto it_parent = child.FindParentBlock(&parent); it_parent == child._parent_blocks.end()) {
			parent._child_blocks.emplace_back(_Child_Block_Info{ &child, child_position, child_width });
			child._parent_blocks.emplace_back(_Parent_Block_Info{ &parent, --parent._child_blocks.end() });
		}
	}
	void DestroyAbstractRelation(Block& parent, Block& child) {
		if (auto it_parent = child.FindParentBlock(&parent); it_parent != child._parent_blocks.end()) {
			parent._child_blocks.erase(it_parent->iterator);
			child._parent_blocks.erase(it_parent);
			if (child.GetCurrentParent() == &parent) { child.SetCurrentParent(nullptr); }
		}
	}
};