#pragma once

#include "core.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)


using std::wstring;


//#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ __FILE__


// Display an error dialog, and exit the program.
void _Error(const char* file,
			const char* function,
			int line, 
			const wstring& description = L"");

#define Error(...)	_Error(__FILENAME__, __func__ , __LINE__, ##__VA_ARGS__)


// Display an exception dialog, but not exit the program.
void _ExceptionDialog(const char* file,
					  const char* function,
					  int line,
					  const wstring& description = L"");

#define ExceptionDialog(...) _ExceptionDialog(__FILENAME__, __func__ , __LINE__, ##__VA_ARGS__)


END_NAMESPACE(WndDesign)
