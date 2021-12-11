#pragma once

#include "../WndDesign.h"


BEGIN_NAMESPACE(WndDesignHelper)


using ulonglong = unsigned long long;


class File : Uncopyable {
public:
	enum class CreateMode : uint {	//	| already existing	|  not existing	|  
		CreateAlways = 2,			//	| 		clear		|	  create	|
		OpenAlways = 4,				//	| 		open		|	  create	|
		CreateNew = 1,				//	| 	    error		|	  create	|
		OpenExisting = 3,			//	| 		open		|	  error		|
		TruncateExisting = 5		//	| 		clear		|	  error		|
	};							 
	enum class AccessMode : uint { 	 
		ReadOnly = 0x80000000L,		// GENERIC_READ
		ReadWrite = 0xC0000000L,	// GENERIC_READ | GENERIC_WRITE
	};							 
	enum class ShareMode : uint {		 
		None = 0x00000000,			// NULL
		ReadOnly = 0x00000001,		// FILE_SHARE_READ
		ReadWrite = 0x00000003,		// FILE_SHARE_READ | FILE_SHARE_WRITE
	};

private:
	Handle _file;
	ulonglong _size;

	CreateMode _create_mode = CreateMode::CreateNew;
	AccessMode _access_mode = AccessMode::ReadOnly;
	ShareMode _share_mode = ShareMode::None;

private:
	bool UpdateFileSize();

protected:
	File(const wstring& file = L"",
		 CreateMode create_mode = CreateMode::OpenAlways, 
		 AccessMode access_mode = AccessMode::ReadWrite, 
		 ShareMode share_mode = ShareMode::None);
	~File() { Close(); }

	bool Open(const wstring& file,
			  CreateMode create_mode = CreateMode::OpenAlways,
			  AccessMode access_mode = AccessMode::ReadWrite,
			  ShareMode share_mode = ShareMode::None);
	void Close();

	bool IsValid() const;
	ulonglong GetSize() const { return _size; }
	bool SetSize(ulonglong size);

	//void SetPointer(uint offset_from_beginning);
	//// Set end of file at current pointer position.
	//void SetEnd(); 
	//// Write size bytes to buffer.
	//void Write(void* buffer, uint size);
	//void Read(void* buffer, uint size);


	////    File Mapping    ////
private:
	Handle _file_map;
	void* _address;

public:
	bool DoFileMapping();
	void UndoFileMapping();
	bool IsMapped() const { return _address != nullptr; }
	void* GetAddress() const { return _address; }
};


END_NAMESPACE(WndDesignHelper)