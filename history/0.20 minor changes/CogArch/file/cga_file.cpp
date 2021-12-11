#include "cga_file.h"

using WndDesignHelper::Point;
using WndDesignHelper::uint;


#include "cga_format_0_0.h"

#include "../block/block_manager.h"

#include <map>


namespace CGAFormat {

using namespace v0_0;


void CGAFile::LoadBlockManager(BlockManager& block_manager) {
	do {
		if (GetSize() == 0) { break; }
		if (!DoFileMapping()) { break; }

		char* data = static_cast<char*>(GetAddress());
		uint size = static_cast<uint>(GetSize());
		uint offset = 0;

		////    Locate header    ////
		if (size < offset + sizeof(Header)) { break; }
		Header* header = reinterpret_cast<Header*>(data + offset);
		if (header->_signature != Header::signature) { break; }
		if (header->_version_high != Header::version_high) { break; }
		if (header->_version_low != Header::version_low) { break; }
		if (header->_table_number != Header::table_number) { break; }
		if (header->_file_size != static_cast<uint>(size)) { break; }

		////    Locate table entries    ////
		offset += sizeof(Header);
		if (size < offset + sizeof(TableEntry) * header->_table_number) { break; }
		TableEntry* tables = reinterpret_cast<TableEntry*>(data + offset);
		std::map<uint, uint> table_map;
		for (uint i = 0; i < header->_table_number; ++i) {
			auto [it, success] = table_map.insert(std::make_pair(tables[i]._signature, i));
			// !!! Table signatures must not be identical. !!!
			if (success == false) { std::abort(); }
		}

		BlockTableEntry* block_table;
		ContentTableEntry* content_table;
		ParentReferenceTableEntry* parent_reference_table;

		// Helper function for loading specific tables entries.
		auto load_table = [&table_map, &tables, &size](auto& table) {
			if (auto it = table_map.find(table->signature); it != table_map.end()) {
				table = static_cast<std::remove_reference_t<decltype(table)>>(&tables[it->second]);
				if (size < table->_offset + table->_size) { return false; }
				return true;
			} 
			return false;
		};
		if (load_table(block_table) == false) { break; }
		if (load_table(content_table) == false) { break; }
		if (load_table(parent_reference_table) == false) { break; }

		////    Locate block table    ////
		offset = block_table->_offset;
		Block* blocks = reinterpret_cast<Block*>(data + offset);
		if (block_table->_size % sizeof(Block) != 0) { break; }
		uint block_count = block_table->_size / sizeof(Block);


		////    Load blocks to block manager    ////
		block_manager.Clear();

		// Save block iterators for random access.
		vector<::Block*> block_list(block_count);
		// Insert blocks.
		for (uint i = 0; i < block_count; ++i) {
			block_list[i] = &block_manager.CreateBlock();
		}
		// Write block contents.
		for (uint i = 0; i < block_count; ++i) {
			////    Locate content    ////
			offset = content_table->_offset + blocks[i].content_offset;
			if (size < offset + blocks[i].content_size) { goto Error; }
			wchar_t* content = reinterpret_cast<wchar_t*>(data + offset);
			uint content_count = blocks[i].content_size / sizeof(wchar_t);
			// Write content.
			block_list[i]->Content().assign(content, content_count);

			////    Locate parents    ////
			offset = parent_reference_table->_offset + blocks[i].parent_offset;
			if (size < offset + blocks[i].parent_size) { goto Error; }
			ParentReference* parents = reinterpret_cast<ParentReference*>(data + offset);
			uint parent_count = blocks[i].parent_size / sizeof(ParentReference);
			// Write parent blocks.
			for (uint parent_index = 0; parent_index < parent_count; parent_index++) {
				uint parent = parents[parent_index].parent;
				if (parent >= block_count) { goto Error; }
				block_manager.CreateInheritanceRelation(
					*block_list[parent], *block_list[i], parents[parent_index].position, parents[parent_index].width
				);
			}
		}

		return;

	} while (0);

Error:
	block_manager.Clear();
	UndoFileMapping();
	return;
}

void CGAFile::SaveBlockManager(const BlockManager& block_manager) {
	do {
		if (!IsValid()) { break; }

		////    Calculate structure information    ////

		// header
		uint header_offset = 0;
		uint header_size = sizeof(Header);

		// table entry
		uint table_entry_offset = header_offset + header_size;
		uint table_entry_size = sizeof(TableEntry) * Header::table_number;

		// block table
		uint block_table_offset = table_entry_offset + table_entry_size;
		uint block_count = block_manager.Blocks().size();
		uint block_table_size = sizeof(Block) * block_count;

		// Calculate content and parent reference size.
		uint content_offset = 0;
		uint parent_reference_offset = 0;
		uint block_index = 0;
		for (auto& it : block_manager.Blocks()) {
			it._block_index = block_index++;  // save block index for quick access.
			uint content_size = it.Content().length() * sizeof(wchar_t);
			uint parent_reference_size = it.ParentBlocks().size() * sizeof(ParentReference);
			content_offset += content_size;
			parent_reference_offset += parent_reference_size;
		}

		// content table
		uint content_table_offset = block_table_offset + block_table_size;
		uint content_table_size = content_offset;

		// parent reference table
		uint parent_reference_table_offset = content_table_offset + content_table_size;
		uint parent_reference_table_length = parent_reference_offset;

		uint file_size = parent_reference_table_offset + parent_reference_table_length;


		////    Write file    ////
		if (!SetSize(file_size)) { break; }
		if (!DoFileMapping()) { break; }
		char* data = static_cast<char*>(GetAddress());

		// Write header.
		Header* header = reinterpret_cast<Header*>(data + header_offset);
		*header = Header(file_size);

		// Write table entries.
		TableEntry* tables = reinterpret_cast<TableEntry*>(data + table_entry_offset);
		tables[0] = BlockTableEntry(block_table_offset, block_table_size);
		tables[1] = ContentTableEntry(content_table_offset, content_table_size);
		tables[2] = ParentReferenceTableEntry(parent_reference_table_offset, parent_reference_table_length);

		// Write block table, content table and parent reference table.
		Block* blocks = reinterpret_cast<Block*>(data + block_table_offset);
		content_offset = 0;
		parent_reference_offset = 0;
		block_index = 0;
		for (auto& it : block_manager.Blocks()) {
			uint content_size = it.Content().length() * sizeof(wchar_t);
			uint parent_reference_size = it.ParentBlocks().size() * sizeof(ParentReference);

			// Write block table.
			blocks[block_index++] = { content_size, content_offset, parent_reference_size, parent_reference_offset };

			// Write content table.
			memcpy(data + content_table_offset + content_offset, it.Content().data(), content_size);

			// Write parent reference table.
			uint offset = parent_reference_table_offset + parent_reference_offset;
			ParentReference* parents = reinterpret_cast<ParentReference*>(data + offset);
			for (uint parent_index = 0; parent_index < it.ParentBlocks().size(); ++parent_index) {
				parents[parent_index] = ParentReference{
					it.ParentBlocks()[parent_index].parent->_block_index,
					it.ParentBlocks()[parent_index].iterator->child_position,
					it.ParentBlocks()[parent_index].iterator->child_width
				};
			}

			content_offset += content_size;
			parent_reference_offset += parent_reference_size;
		}

		return;

	} while (0);

//Error:

	// Show exception dialog. (and clear the file?)

	return;
}


}  // namespace CGAFormat