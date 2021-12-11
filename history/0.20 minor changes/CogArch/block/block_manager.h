#pragma once

#include "block.h"

#include <algorithm>



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

	// Should automatically clear those isolated empty blocks.

	//void DestroyBlock(Block& block) {
	//	// First clear all relationships.


	//	// Then remove it.
	//	_blocks.erase(block._iterator);
	//}

public:
	// Inheritance relation.
	void CreateInheritanceRelation(Block& parent, Block& child, Point child_position, uint child_width) {
		auto it_parent = std::lower_bound(child._parent_blocks.begin(), child._parent_blocks.end(), &parent);
		if (it_parent != child._parent_blocks.end() && it_parent->parent == &parent) { return; } // Already inserted!

		parent._child_blocks.emplace_front(_Child_Block_Info{ &child, child_position, child_width});
		child._parent_blocks.emplace(it_parent, _Parent_Block_Info{ &parent, parent._child_blocks.begin() });
		
		// To do: For large abstract parent blocks, child block references may not be kept.

	}
	void DestroyInheritanceRelation(Block& parent, Block& child) {
		auto it_parent = std::lower_bound(child._parent_blocks.begin(), child._parent_blocks.end(), &parent);
		if (it_parent == child._parent_blocks.end() || it_parent->parent != &parent) { return ; } // No existing relationship!

		if (it_parent->iterator != parent._child_blocks.end()) { parent._child_blocks.erase(it_parent->iterator); }
		child._parent_blocks.erase(it_parent);
	}
	//bool HasInheritanceRelation(Block& parent, Block& child) const {
	//	auto it_parent = std::lower_bound(child._parent_blocks.begin(), child._parent_blocks.end(), &parent);
	//	if (it_parent == child._parent_blocks.end() || it_parent->parent != &parent) { return false; }
	//	return true;
	//}

	// Dependence relation.
	// ...
};