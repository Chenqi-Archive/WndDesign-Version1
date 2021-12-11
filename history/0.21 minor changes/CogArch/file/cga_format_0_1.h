#pragma once

#include "cga_format.h"

namespace CGAFormat::v0_1 {


DECLARE_HEADER(0, 0, 4);

DECLARE_TABLE_ENTRY(BlockTableEntry, 'BLOC');
DECLARE_TABLE_ENTRY(ClipboardTableEntry, 'CLIP');
DECLARE_TABLE_ENTRY(ContentTableEntry, 'CONT');
DECLARE_TABLE_ENTRY(ParentReferenceTableEntry, 'PARE');


struct BlockTable {
	// Block blocks[_size / sizeof(Block)];
};

struct ClipboardTable {
	// uint blocks_in_clipboard[_size / sizeof(uint)];
};

struct ContentTable {
	// wchar_t content[_size / sizeof(wchar_t)];
};

struct ParentReferenceTable {
	// ParentReference parents[_size / sizeof(ParentReference)];
};


struct Block {
	uint content_size;
	uint content_offset;		// relative to ContentTable

	uint parent_size;
	uint parent_offset;			// relative to ParentReferenceTable
};

struct ParentReference {
	uint parent;
	Point position;
	uint width;
};


}  // namespace CGAFormat::v0_1