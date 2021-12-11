#include "metrics.h"

#undef Alloc
#include <Windows.h>

BEGIN_NAMESPACE(WndDesign)

const Size GetDesktopSize() {
	static Size size = size_min;
	if (size == size_min) {
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int cx = rect.right - rect.left;
		int cy = rect.bottom - rect.top;
		size = { static_cast<uint>(cx), static_cast<uint>(cy) };
	}
	return size;
}

END_NAMESPACE(WndDesign)
