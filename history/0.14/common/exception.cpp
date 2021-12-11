#include "exception.h"

#undef Alloc
#include <Windows.h>

BEGIN_NAMESPACE(WndDesign)

inline wstring StringToWString(const char* text) {
	int length = MultiByteToWideChar(CP_ACP, NULL, text, -1, nullptr, 0);
	wchar_t* wtext = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, NULL, text, -1, wtext, length);
	std::wstring str(wtext);
	delete[] wtext;
	return str;
}


#define BufferCnt 256

void _Error(const char* file, const char* function, int line, const wstring& description) {
	wchar error_info[BufferCnt];
	swprintf_s(error_info, BufferCnt, L"An error occurred: %s\n  File: %s\n  Function: %s\n  Line: %d\n",
			   description.c_str(), StringToWString(file).c_str(), StringToWString(function).c_str(), line);
	MessageBox(NULL, error_info, L"Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	abort();
}


void _ExceptionDialog(const char* file, const char* function, int line, const wstring& description) {
	_Error(file, function, line, description);
}


END_NAMESPACE(WndDesign)