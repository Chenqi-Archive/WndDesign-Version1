#include "exception.h"

#include <Windows.h>

BEGIN_NAMESPACE(WndDesign)

BEGIN_NAMESPACE(Anonymous)

inline wstring StringToWString(const char* text) {
	int length = MultiByteToWideChar(CP_ACP, NULL, text, -1, nullptr, 0);
	wchar_t* wtext = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, NULL, text, -1, wtext, length);
	std::wstring str(wtext);
	delete[] wtext;
	return str;
}

const uint buffer_size = 256;

bool is_exception_dialog_disabled = false;

END_NAMESPACE(Anonymous)


void _Error(const char* file, const char* function, int line, const wstring& description) {
	wchar error_info[buffer_size];
	swprintf_s(error_info, buffer_size, L"An error occurred: %s\n  File: %s\n  Function: %s\n  Line: %d\n",
			   description.c_str(), StringToWString(file).c_str(), StringToWString(function).c_str(), line);
	MessageBox(NULL, error_info, L"Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	abort();
}

void _ExceptionDialog(const char* file, const char* function, int line, const wstring& description) {
	if (is_exception_dialog_disabled) { return; }

	wchar error_info[buffer_size];
	swprintf_s(error_info, buffer_size, L"An error occurred: %s\n  File: %s\n  Function: %s\n  Line: %d\n",
			   description.c_str(), StringToWString(file).c_str(), StringToWString(function).c_str(), line);
	
	// Or use a customized dialog.
	MessageBox(NULL, error_info, L"Error", MB_OK | MB_ICONWARNING | MB_TASKMODAL);
}

void DisableExceptionDialog() {
	is_exception_dialog_disabled = true;
}

void EnableExceptionDialog() {
	is_exception_dialog_disabled = false;
}


END_NAMESPACE(WndDesign)