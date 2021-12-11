#pragma once

#include "cga_format.h"

#include "../WndDesign.h"

using WndDesignHelper::File;

class BlockManager;


namespace CGAFormat {


class CGAFile : private File {
public:
	CGAFile() :File() {}
	~CGAFile() {}
	using File::Open;
	using File::Close;
	using File::IsValid;

public:
	// Load file to block manager, if error occured, the block manager is left unchanged.
	void LoadBlockManager(BlockManager& block_manager);
	// Save block manager to file, the old file content will be overwritten.
	void SaveBlockManager(const BlockManager& block_manager);
};


}  // namespace CGAFormat