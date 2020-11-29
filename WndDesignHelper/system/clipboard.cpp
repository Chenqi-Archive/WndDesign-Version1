#include "clipboard.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesignHelper)


void SetClipboardData(const wstring& text) {
	bool res;
	res = OpenClipboard(NULL);
	if (!res) { return; }
	EmptyClipboard();

	HGLOBAL hdata = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (text.length() + 1) * sizeof(wchar));
	wchar* data = reinterpret_cast<wchar*>(GlobalLock(hdata));
	wcscpy_s(data, text.length() + 1, text.data());
	GlobalUnlock(hdata);

	res = ::SetClipboardData(CF_UNICODETEXT, hdata);
	if (!res) { GlobalFree(hdata); }

	CloseClipboard();
}

void GetClipboardData(wstring& text) {
	bool res;
	res = OpenClipboard(NULL);
	if (!res) { text.clear(); return; }

	HANDLE hdata = ::GetClipboardData(CF_UNICODETEXT);
	if (hdata == NULL) {
		text.clear();
	} else {
		const wchar* data = reinterpret_cast<const wchar*>(GlobalLock(hdata));
		text = data;
		GlobalUnlock(hdata);
	}

	CloseClipboard();
}


END_NAMESPACE(WndDesignHelper)