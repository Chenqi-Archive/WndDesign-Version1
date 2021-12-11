#include "metrics.h"

#include <Windows.h>

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API const Size GetDesktopSize() {
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;
	Size size = Size(static_cast<uint>(cx), static_cast<uint>(cy));
	return size;
}


END_NAMESPACE(WndDesign)
