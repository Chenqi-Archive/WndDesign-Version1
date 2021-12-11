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


namespace CGAFormat { class CGAFile; }

class Block;


struct _Child_Block_Info {
	Block* child;
	Point child_position;
	uint child_width;  // height will auto resize
};

struct _Parent_Block_Info {
	Block* parent;
	list<_Child_Block_Info>::iterator iterator;
	bool operator<(Block* other) const { return parent < other; }
	bool operator<(const _Parent_Block_Info& other) const { return parent < other.parent; }
	operator Block* () const { return parent; }
};

inline static constexpr Point child_position_auto = Point(0x7FFFFFFF, 0x7FFFFFFF);
inline static constexpr uint child_width_auto = 0xFFFFFFFF;


class Block : Uncopyable {
private:
	friend class BlockManager;
	list<Block>::iterator _iterator;  // For quick locating my position in block manager.

public:
	mutable uint _block_index = 0;  // temporapy value used when saving to file

private:
	wstring _content;
	vector<_Parent_Block_Info> _parent_blocks;  // sorted by pointer value
	list<_Child_Block_Info> _child_blocks;


	//vector<Block*> dependent_blocks;
	//vector<Block*> referenced_blocks;

public:
	auto& Content() { return _content; }
	const auto& Content() const { return _content; }
	const auto& ChildBlocks() const { return _child_blocks; }
	const auto& ParentBlocks() const { return _parent_blocks; }
};