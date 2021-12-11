 #define d2d

#ifdef d2d

#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")

HWND hWnd;

extern void Init();
extern void Final();
extern void OnPaint();
extern void OnSize(int, int);
extern void OnHover(int, int);
extern void OnLeftDown(int, int);
extern void OnSelect(int x1, int y1, int x2, int y2);

enum class CaretState { None, Static, BlinkShow, BlinkHide };
extern CaretState caret_state;



LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	const wchar_t className[] = L"D2DTestClass";
	const wchar_t titleName[] = L"D2DTest";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // ���ӵĻ�������ڴ�С�仯����ֺ�ɫ
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = className;

	if (!RegisterClassEx(&wc)) {
		return 0;
	}

	hWnd = CreateWindowEx(NULL,
						  className, titleName,
						  WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
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
	case WM_CREATE: {
			DWM_BLURBEHIND bb = { 0 };

			// Specify blur-behind and blur region.
			bb.dwFlags = DWM_BB_ENABLE;
			bb.fEnable = true;
			bb.hRgnBlur = NULL;

			// Enable blur-behind.
			DwmEnableBlurBehindWindow(hWnd, &bb); 
		}break;
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
//		OnSize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND: // There may be problem using driect2d if not intercept WM_ERASEBKGND from DefWindowProc.
		break;
	case WM_DWMCOMPOSITIONCHANGED: {
			MARGINS margins = { 0,0,0,25 };
			// Extend frame on the bottom of client area
			DwmExtendFrameIntoClientArea(hWnd, &margins);
		}break;
	case WM_LBUTTONDOWN:
		has_down = true;
		down_pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		OnLeftDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (caret_state != CaretState::None) {
			timer_time = 0;
			timer = SetTimer(hWnd, timer, timer_period, nullptr);
		}
		SetActiveWindow(hWnd);
		SetFocus(hWnd);
		SetCapture(hWnd);
		break;
	case WM_MOUSEMOVE:
		if (has_down) {
			//RECT rect;
			//GetWindowRect(hWnd, &rect);
			//POINT cur_pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			//MoveWindow(hWnd, rect.left + cur_pos.x - down_pos.x, rect.top + cur_pos.y - down_pos.y, 800, 500, false);
			OnSelect(down_pos.x, down_pos.y, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		OnHover(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		has_down = false;
		ReleaseCapture();
		break;

	case WM_MOUSEHWHEEL:
		break;
	case WM_MOUSEWHEEL:
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

	case WM_DESTROY:
		KillTimer(hWnd, timer);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}


#endif