#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <windowsx.h>

//#define CONSOLE_DEBUG

#ifdef CONSOLE_DEBUG
#include <cstdio>
void InitConsole() {
	AllocConsole();
	freopen("CON", "r", stdin);
	freopen("CON", "w", stdout);
}
void FinishConsole() {
	FreeConsole();
}
#else
#define InitConsole()
#define FinishConsole()
#endif


HWND hWnd;

using uint = unsigned int;

extern void Init();
extern void Final();
extern void OnPaint();
extern void OnSize(uint, uint);
extern void OnUpdate();
extern void OnDrag(int dx, int dy);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	InitConsole();

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

	FinishConsole();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static bool has_mouse_down = false;
	static POINT mouse_down_pos;
	UINT_PTR timer = 0;
	switch (msg) {
		case WM_CREATE:
			timer = SetTimer(hWnd, 203, 1000, nullptr);
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
		case WM_LBUTTONDOWN:
			has_mouse_down = true;
			mouse_down_pos = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
			break;
		case WM_LBUTTONUP:
			has_mouse_down = false;
			break;
		case WM_MOUSEMOVE:
			if (has_mouse_down) {
				POINT current_pos = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
				OnDrag(current_pos.x - mouse_down_pos.x, current_pos.y - mouse_down_pos.y);
				mouse_down_pos = current_pos;
			}
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
