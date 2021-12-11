#include "win32_api.h"

#include "../window/desktop_impl.h"

#undef Alloc
#include <Windows.h>
#include <windowsx.h>

BEGIN_NAMESPACE(WndDesign)
BEGIN_NAMESPACE(Win32)

inline RECT Rect2RECT(Rect rect) {
    return RECT{ static_cast<LONG>(rect.point.x), static_cast<LONG>(rect.point.y),
                 static_cast<LONG>(rect.RightBottom().x), static_cast<LONG>(rect.RightBottom().y) };
}

inline Rect RECT2Rect(RECT rect) {
    Rect _rect;
    _rect.point.x = static_cast<int>(rect.left);
    _rect.point.y = static_cast<int>(rect.top);
    _rect.size.width = static_cast<uint>(rect.right - rect.left);
    _rect.size.height = static_cast<uint>(rect.bottom - rect.top);
    return _rect;
}

namespace {

bool has_wnd_class_registered = false;
const wchar_t wnd_class_name[] = L"WndDesignFrame";
int window_cnt = 0;
extern HINSTANCE hInstance;


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Get the attached layer.
    Ref SystemWndLayer* layer = reinterpret_cast<Ref SystemWndLayer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    // Get the mouse message parameter.
    MouseMsg mouse_msg;
    mouse_msg.point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    mouse_msg.virtual_keys = GET_KEYSTATE_WPARAM(wParam);;
    mouse_msg.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);

    switch (msg) {
    case WM_CREATE: {
            // Store the pointer of the attached layer as the user data.
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            Ref SystemWndLayer* layer = reinterpret_cast<Ref SystemWndLayer*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)layer);
            window_cnt++;
        }break;
    case WM_DESTROY: {
            window_cnt--;
            if (window_cnt == 0) { PostQuitMessage(0); }
        }break;
    case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            layer->RegionUpdated(RECT2Rect(ps.rcPaint));
            EndPaint(hWnd, &ps);
        }break;
    case WM_ERASEBKGND: 
        break;
    case WM_MOUSEMOVE:
        SetCapture(hWnd);
        layer->ReceiveSystemMessage(Msg::MouseMove, &mouse_msg); 
        break;
    case WM_MOUSEWHEEL: 
        layer->ReceiveSystemMessage(Msg::MouseWheel, &mouse_msg); 
        ReleaseCapture(); 
        break;
    case WM_LBUTTONDOWN: 
        layer->ReceiveSystemMessage(Msg::LeftDown, &mouse_msg); 
        break;
    case WM_LBUTTONUP: 
        layer->ReceiveSystemMessage(Msg::LeftUp, &mouse_msg); 
        break;
    case WM_RBUTTONDOWN: 
        layer->ReceiveSystemMessage(Msg::RightDown, &mouse_msg); 
        break;
    case WM_RBUTTONUP: 
        layer->ReceiveSystemMessage(Msg::RightUp, &mouse_msg); 
        break;
    default: 
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void RegisterWndClass() {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = wnd_class_name;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    ATOM res = RegisterClassExW(&wcex);
}

}



HANDLE CreateWnd(Ref SystemWndLayer* layer, Rect region, WndStyle::RenderStyle style) {
    if (!has_wnd_class_registered) { RegisterWndClass(); }
    HWND hWnd = CreateWindowExW(NULL, wnd_class_name, L"", WS_POPUP,
                                region.point.x, region.point.y, region.size.width, region.size.height,
                                NULL, NULL, hInstance, layer);
    if (style._opacity != 0xFF) {
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, RGB(0xFF, 0xFF, 0xFF), style._opacity, LWA_ALPHA);
    }
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    return hWnd;
}

void DestroyWnd(HANDLE hWnd) {
    DestroyWindow(static_cast<HWND>(hWnd));
}

void MoveWnd(HANDLE hWnd, Rect region) {
    MoveWindow(static_cast<HWND>(hWnd), region.point.x, region.point.y, region.size.width, region.size.height, false);
}

void SetWndStyle(HANDLE hWnd, WndStyle::RenderStyle style) {
    //HWND hwnd = static_cast<HWND>(hWnd);
    //if (style._opacity != 0xFF) {
    //    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    //    SetLayeredWindowAttributes(hwnd, RGB(0xFF, 0xFF, 0xFF), style._opacity, LWA_ALPHA);
    //}
}

int MessageLoop() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}


END_NAMESPACE(Win32)
END_NAMESPACE(WndDesign)
