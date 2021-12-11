#pragma once

#include "core.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)


using std::wstring;


inline void _BreakPoint() { 

}
#define BreakPoint() _BreakPoint()

// Display an exception dialog, and exit the program.
inline void ExceptionDialog(const wstring& description = L"") {
	BreakPoint();
	std::abort();
}

END_NAMESPACE(WndDesign)
