#include "file.h"


#include <Windows.h>

BEGIN_NAMESPACE(WndDesignHelper)


File::File(const wstring& file, CreateMode create_mode, AccessMode access_mode, ShareMode share_mode) :
    _file(INVALID_HANDLE_VALUE),
    _size(0),

    _file_map(NULL),
    _address(nullptr) {

    Open(file, create_mode, access_mode, share_mode);
}

bool File::Open(const wstring& file, CreateMode create_mode, AccessMode access_mode, ShareMode share_mode) {
    if (IsValid()) { Close(); }
    if (file.empty()) { return false; }
    do {
        _create_mode = create_mode;
        _access_mode = access_mode;
        _share_mode = share_mode;

        _file = ::CreateFileW(file.c_str(),
                              static_cast<DWORD>(access_mode),
                              static_cast<DWORD>(share_mode),
                              NULL,
                              static_cast<DWORD>(create_mode),
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
        if (_file == INVALID_HANDLE_VALUE) { break; }

        if (UpdateFileSize() == false) { break; }

        return true;

    } while (0);

//Error:
    Close();
    return false;
}

void File::Close() {
    UndoFileMapping();
    _size = 0;
    if (_file != INVALID_HANDLE_VALUE) { ::CloseHandle(_file); _file = INVALID_HANDLE_VALUE; }
}

bool File::IsValid() const { return _file != INVALID_HANDLE_VALUE; }


bool File::UpdateFileSize() {
    if (!IsValid()) { return false; }
    static_assert(sizeof(ulonglong) == sizeof(LARGE_INTEGER));
    if (::GetFileSizeEx(_file, reinterpret_cast<PLARGE_INTEGER>(&_size)) == FALSE) { return false; }
    return true;
}

bool File::SetSize(ulonglong size) {
    if (!IsValid()) { return false; }

    do {
        UndoFileMapping();

        LARGE_INTEGER new_size; new_size.QuadPart = size;
        if (SetFilePointerEx(_file, new_size, NULL, FILE_BEGIN) == false) { break; }

        if (SetEndOfFile(_file) == false) { break; }

        UpdateFileSize();

        return true;

    } while (0);

//Error:
    Close();
    return false;
}


bool File::DoFileMapping() {
    if (!IsValid()) { return false; }
    if (IsMapped()) { return true; }

    do {
        _file_map = ::CreateFileMappingW(_file,
                                         NULL,
                                         _access_mode == AccessMode::ReadOnly ? PAGE_READONLY : PAGE_READWRITE,
                                         static_cast<DWORD>(_size >> 32), static_cast<DWORD>(_size),
                                         NULL);
        if (_file_map == NULL) { break; }

        _address = ::MapViewOfFile(_file_map,
                                   _access_mode == AccessMode::ReadOnly ? FILE_MAP_READ : (FILE_MAP_READ | FILE_MAP_WRITE),
                                   0, 0, 0);
        if (_address == nullptr) { break; }

        return true;

    } while (0);

//Error:
    UndoFileMapping();
    return false;
}

void File::UndoFileMapping() {
    if (_address != nullptr) { ::UnmapViewOfFile(_address); _address = nullptr; }
    if (_file_map != NULL) { ::CloseHandle(_file_map); _file_map = NULL; }
}


END_NAMESPACE(WndDesignHelper)