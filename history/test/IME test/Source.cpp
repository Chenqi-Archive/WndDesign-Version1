
#include "ime_input.h"

#include <Windows.h>
#include <windowsx.h>

HWND hWnd;

ImeInput ime;

extern void Init();
extern void Final();
extern void OnPaint();
extern void OnSize(int, int);
extern void OnHover(int, int);
extern void OnLeftDown(int, int);
extern void OnSelect(int x1, int y1, int x2, int y2);

enum class CaretState { None, Static, BlinkShow, BlinkHide };
extern CaretState caret_state;

void SetCaret(Rect caret_region) {
	ime.UpdateCaretRect(hWnd, caret_region);
}



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
	static UINT_PTR timer = 12345;
	static const UINT timer_period = 500;
	static UINT timer_time = 0;
	static bool has_down = false;
	static POINT down_pos = {};

	switch (msg) {
	case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			OnPaint();
			EndPaint(hWnd, &ps);
		}break;
	case WM_TIMER:
		if (caret_state == CaretState::BlinkShow) { caret_state = CaretState::BlinkHide; OnPaint(); }
		else if (caret_state == CaretState::BlinkHide) { caret_state = CaretState::BlinkShow; OnPaint(); }
		timer_time += timer_period;
		if (timer_time >= 10000) {
			KillTimer(hWnd, timer);
			caret_state = CaretState::Static; OnPaint();
		}
		break;
	case WM_SIZE:
		OnSize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND: // There may be problem using driect2d if not intercept WM_ERASEBKGND from DefWindowProc.
		break;
	case WM_LBUTTONDOWN:
		has_down = true;
		down_pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		OnLeftDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (caret_state != CaretState::None) {
			timer_time = 0;
			timer = SetTimer(hWnd, timer, timer_period, nullptr);
		}
		break;
	case WM_MOUSEMOVE:
		if (has_down) {
			OnSelect(down_pos.x, down_pos.y, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		OnHover(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		has_down = false;
		break;
	case WM_KEYDOWN:
		break;
	case WM_CHAR:
		break;
	case WM_UNICHAR:
		if (wParam == UNICODE_NOCHAR) { return true; } // A test message, return true to show that the program processes the message.
		break;
	case WM_KEYUP:
		break;

	// Input method:
	case WM_IME_SETCONTEXT: {
			BOOL handled;
			ime.CreateImeWindow(hWnd);
			ime.CleanupComposition(hWnd);
			ime.SetImeWindowStyle(hWnd, msg, wParam, lParam, &handled);
		}break;
	case WM_IME_STARTCOMPOSITION:
		ime.CreateImeWindow(hWnd);
		ime.ResetComposition(hWnd);
		break;
	case WM_IME_COMPOSITION: {
			ImeComposition result_string = {}, composition_string = {};
			ime.UpdateImeWindow(hWnd);
			ime.GetResult(hWnd, lParam, &result_string);
			ime.GetComposition(hWnd, lParam, &composition_string);
			//ime.ResetComposition(hWnd);
		}break;
	case WM_IME_ENDCOMPOSITION:
		ime.ResetComposition(hWnd);
		ime.DestroyImeWindow(hWnd);
		break;
	case WM_INPUTLANGCHANGE:
		ime.SetInputLanguage();
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
