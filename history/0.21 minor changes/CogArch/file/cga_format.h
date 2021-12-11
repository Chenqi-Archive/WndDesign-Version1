#pragma once


namespace CGAFormat {

using uint = unsigned int;
using ushort = unsigned short;


struct HeaderCommon {
	inline static const uint signature = 'CGAF';

	uint _signature;
	ushort _version_high;
	ushort _version_low;
	uint _file_size;

	uint _table_number;
	// TableEntry tables[table_number];

	constexpr HeaderCommon(ushort version_high, ushort version_low, uint file_size, uint table_number) :
		_signature(signature), 
		_version_high(version_high), 
		_version_low(version_low), 
		_file_size(file_size), 
		_table_number(table_number) {
	}
};


struct TableEntry {
	uint _signature;
	uint _offset;		  // relative to file head.
	uint _size;
	constexpr TableEntry(uint signature, uint offset, uint size) :
		_signature(signature), _offset(offset), _size(size) {
	}
};


#define DECLARE_HEADER(v_high, v_low, t_number) \
	struct Header : public HeaderCommon { \
		inline static const ushort version_high = v_high; \
		inline static const ushort version_low = v_low; \
		inline static const uint table_number = t_number; \
		Header(uint file_size) :HeaderCommon(version_high, version_low, file_size, table_number) {} \
	}

// !!! Table signatures must not be identical. !!!
#define DECLARE_TABLE_ENTRY(name, sig)  \
	struct name : TableEntry { \
		inline static const uint signature = sig; \
		constexpr name(uint offset, uint size) :TableEntry(signature, offset, size) {} \
	}


}