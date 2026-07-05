#include "toggle_control.h"

#include "state.h"
#include "window_utils.h"

namespace eef {

void DrawCheckmark(HWND window, HDC dc, const RECT& boxRect)
{
    HPEN pen = CreatePen(PS_SOLID, ScaleForWindow(window, 2),
                         RGB(255, 255, 255));
    HGDIOBJ oldPen = SelectObject(dc, pen);

    const int left = boxRect.left;
    const int top = boxRect.top;
    MoveToEx(dc, left + ScaleForWindow(window, 4),
             top + ScaleForWindow(window, 8), nullptr);
    LineTo(dc, left + ScaleForWindow(window, 7),
           top + ScaleForWindow(window, 11));
    LineTo(dc, left + ScaleForWindow(window, 13),
           top + ScaleForWindow(window, 5));

    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

void PaintToggleControl(HWND window, HDC dc)
{
    RECT rect;
    GetClientRect(window, &rect);

    HBRUSH chromeBrush = CreateSolidBrush(RGB(16, 16, 16));
    FillRect(dc, &rect, chromeBrush);
    DeleteObject(chromeBrush);

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(31, 31, 31));
    HPEN borderPen = CreatePen(PS_SOLID, ScaleForWindow(window, 1),
                               RGB(75, 75, 75));
    HGDIOBJ oldBrush = SelectObject(dc, backgroundBrush);
    HGDIOBJ oldPen = SelectObject(dc, borderPen);

    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom,
              ScaleForWindow(window, 8), ScaleForWindow(window, 8));

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(backgroundBrush);
    DeleteObject(borderPen);

    const int boxSize = ScaleForWindow(window, 16);
    RECT boxRect = {
        ScaleForWindow(window, 10),
        (rect.bottom - boxSize) / 2,
        ScaleForWindow(window, 10) + boxSize,
        (rect.bottom - boxSize) / 2 + boxSize,
    };

    const bool enabled = g_enabled.load();
    HBRUSH boxBrush = CreateSolidBrush(enabled ? RGB(0, 95, 184)
                                               : RGB(42, 42, 42));
    HPEN boxPen = CreatePen(PS_SOLID, ScaleForWindow(window, 1),
                            enabled ? RGB(59, 158, 255)
                                    : RGB(122, 122, 122));
    oldBrush = SelectObject(dc, boxBrush);
    oldPen = SelectObject(dc, boxPen);
    RoundRect(dc, boxRect.left, boxRect.top, boxRect.right, boxRect.bottom,
              ScaleForWindow(window, 3), ScaleForWindow(window, 3));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(boxBrush);
    DeleteObject(boxPen);

    if (enabled) {
        DrawCheckmark(window, dc, boxRect);
    }

    RECT textRect = {
        ScaleForWindow(window, 34),
        0,
        rect.right - ScaleForWindow(window, 12),
        rect.bottom,
    };

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(242, 242, 242));
    HGDIOBJ oldFont = SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
    DrawTextW(dc, L"Expandable folders", -1, &textRect,
              DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    SelectObject(dc, oldFont);
}

void SetToggleEnabled(bool enabled)
{
    const bool previous = g_enabled.exchange(enabled);
    if (previous == enabled) {
        return;
    }

    Wh_SetIntValue(kEnabledValueName, enabled ? 1 : 0);
    Wh_Log(L"Explorer toggle changed: enabled=%d", enabled);

    for (ExplorerWindow& window : g_windows) {
        window.lastChecked = enabled;
        if (IsWindow(window.checkbox)) {
            InvalidateRect(window.checkbox, nullptr, FALSE);
        }
    }
}

LRESULT CALLBACK ToggleControlWndProc(HWND window,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam)
{
    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            PaintToggleControl(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;

        case WM_LBUTTONUP:
            SetToggleEnabled(!g_enabled.load());
            return 0;

        case WM_KEYUP:
            if (wParam == VK_SPACE || wParam == VK_RETURN) {
                SetToggleEnabled(!g_enabled.load());
                return 0;
            }
            break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void RegisterToggleControlClass()
{
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = ToggleControlWndProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_HAND);
    windowClass.lpszClassName = kToggleClassName;

    RegisterClassExW(&windowClass);
}

}  // namespace eef
