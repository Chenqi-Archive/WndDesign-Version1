#include <Windows.h>
#include <windowsx.h>

HWND hWnd;

extern void Init();
extern void Final();
extern void OnPaint();
extern void OnSize(int, int);
extern void OnUpdate();

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	const wchar_t className[] = L"CogArchWndClass";
	const wchar_t titleName[] = L"CogArch";

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

	hWnd = CreateWindowEx(NULL,
		className, titleName,
		WS_OVERLAPPEDWINDOW,
		200, 200, 800, 500,
		NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		return 0;
	}

	Init();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Final();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	UINT_PTR timer = 0;
	switch (msg) {
		case WM_CREATE:
			timer = SetTimer(hWnd, 203, 2000, nullptr);
			break;
		case WM_PAINT:
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			OnPaint();
			EndPaint(hWnd, &ps);
			break;
		case WM_TIMER:
			OnUpdate();
			break;
		case WM_SIZE:
			OnSize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_ERASEBKGND:
			return false;
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_DESTROY:
			KillTimer(hWnd, timer);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
