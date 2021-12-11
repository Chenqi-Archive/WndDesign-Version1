#pragma once

#include "core.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)


using std::wstring;


//#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ __FILE__


void _Error(const char* file,
			const char* function,
			int line, 
			const wstring& description = L"");

#ifdef _DEBUG
// Display an error dialog, and exit the program.
#define Error(...)	_Error(__FILENAME__, __func__ , __LINE__, ##__VA_ARGS__)
#else
#define Error()
#endif

void _ExceptionDialog(const char* file,
					  const char* function,
					  int line,
					  const wstring& description = L"");

// Display an exception dialog, but not exit the program.
// You could disable the exception dialog.
#define ExceptionDialog(...) _ExceptionDialog(__FILENAME__, __func__ , __LINE__, ##__VA_ARGS__)

void DisableExceptionDialog();
void EnableExceptionDialog();


END_NAMESPACE(WndDesign)
