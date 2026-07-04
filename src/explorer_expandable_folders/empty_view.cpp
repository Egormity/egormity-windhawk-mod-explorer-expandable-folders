#include "empty_view.h"

namespace eef {

LRESULT CALLBACK EmptyViewWndProc(HWND window,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam)
{
    switch (message) {
        case WM_ERASEBKGND: {
            RECT rect;
            GetClientRect(window, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(16, 16, 16));
            FillRect(reinterpret_cast<HDC>(wParam), &rect, brush);
            DeleteObject(brush);
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            RECT rect;
            GetClientRect(window, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(16, 16, 16));
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            EndPaint(window, &paint);
            return 0;
        }
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void RegisterEmptyViewClass()
{
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = EmptyViewWndProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kOverlayClassName;

    RegisterClassExW(&windowClass);
}

}  // namespace eef
