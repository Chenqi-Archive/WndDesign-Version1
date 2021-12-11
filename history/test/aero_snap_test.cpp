#include <Windows.h>
#include <windowsx.h>

HWND hWnd;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"D2DTestClass";
	const wchar_t titleName[] = L"D2DTest";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 不加的话如果窗口大小变化会出现黑色
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = className;

	if (!RegisterClassEx(&wc)) {
		return 0;
	}

	hWnd = CreateWindowEx(
		NULL,
		className, titleName,
		WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX, // WS_THICKFRAME and WS_MAXIMIZEBOX are necessary for implementing aero snap.
		200, 200, 800, 500,
		NULL, NULL, hInstance, NULL
	);

	if (hWnd == NULL) {
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			FillRect(ps.hdc, &ps.rcPaint, WHITE_BRUSH);
			EndPaint(hWnd, &ps);
		}break;
	case WM_NCCALCSIZE:  // Process the message to set client region the same as the window region.
		break;
	case WM_NCACTIVATE:  // Do not draw the nonclient area.
        break;

	//case WM_NCHITTEST: {
	//		LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
	//		if (hit == HTCLIENT) hit = HTCAPTION;
	//		return hit;
	//	}break;
	case WM_LBUTTONDOWN:  // Send the fake mouse messages hit on caption to fool DefWindowProc().
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		break;
	case WM_MOUSEMOVE:
		//SendMessage(hWnd, WM_NCMOUSEMOVE, HTCAPTION, lParam);// If already left down, the message won't appear! But spy++ really tracked the message. There must be something suspicus with GetMessage().
		break;
	case WM_LBUTTONUP:
		//SendMessage(hWnd, WM_NCLBUTTONUP, HTCAPTION, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
