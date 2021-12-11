#pragma once

#include "cga_format.h"

namespace CGAFormat::v0_0 {


DECLARE_HEADER(0, 0, 3);

DECLARE_TABLE_ENTRY(BlockTableEntry, 'BLOC');
DECLARE_TABLE_ENTRY(ContentTableEntry, 'CONT');
DECLARE_TABLE_ENTRY(ParentReferenceTableEntry, 'PARE');


struct BlockTable {
	// Block blocks[_size / sizeof(Block)];
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


}  // namespace CGAFormat::v0_0