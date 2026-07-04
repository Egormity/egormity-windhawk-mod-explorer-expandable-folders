// ==WindhawkMod==
// @id              explorer-expandable-folders
// @name            Explorer Expandable Folders
// @description     Explorer-hosted scaffold for expandable folders.
// @version         0.3.3
// @author          Egormity
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32
// ==/WindhawkMod==



#include <windows.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <vector>

namespace eef {

constexpr PCWSTR kOverlayClassName =
    L"ExplorerExpandableFoldersEmptyView";
constexpr PCWSTR kToggleClassName =
    L"ExplorerExpandableFoldersToggle";
constexpr PCWSTR kEnabledValueName = L"explorerViewEnabled";

struct ExplorerWindow {
    HWND frame;
    HWND checkbox;
    HWND emptyView;
    bool lastChecked;
};

}  // namespace eef



namespace eef {

extern std::atomic_bool g_running;
extern std::atomic_bool g_enabled;
extern HANDLE g_managerThread;
extern std::vector<ExplorerWindow> g_windows;

}  // namespace eef


namespace eef {

std::atomic_bool g_running = false;
std::atomic_bool g_enabled = false;
HANDLE g_managerThread = nullptr;
std::vector<ExplorerWindow> g_windows;

}  // namespace eef



namespace eef {

int ScaleForWindow(HWND window, int value);
bool IsExplorerFrame(HWND window);
ExplorerWindow* FindWindowState(HWND frame);

}  // namespace eef


namespace eef {

int ScaleForWindow(HWND window, int value)
{
    const UINT dpi = GetDpiForWindow(window);
    return MulDiv(value, dpi ? dpi : 96, 96);
}

bool IsExplorerFrame(HWND window)
{
    wchar_t className[64] = {};
    GetClassNameW(window, className, ARRAYSIZE(className));
    if (wcscmp(className, L"CabinetWClass") != 0) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    return processId == GetCurrentProcessId() && IsWindowVisible(window);
}

ExplorerWindow* FindWindowState(HWND frame)
{
    auto it = std::find_if(
        g_windows.begin(), g_windows.end(),
        [frame](const ExplorerWindow& window) { return window.frame == frame; });

    return it == g_windows.end() ? nullptr : &*it;
}

}  // namespace eef



namespace eef {

LRESULT CALLBACK EmptyViewWndProc(HWND window,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);
void RegisterEmptyViewClass();

}  // namespace eef


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



namespace eef {

void RegisterToggleControlClass();

}  // namespace eef



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



namespace eef {

void DestroyWindowState(ExplorerWindow& window);
void LayoutWindowState(ExplorerWindow& window);
void AddExplorerWindow(HWND frame);
void RemoveClosedWindows();
void SyncCheckboxState();
BOOL CALLBACK EnumExplorerWindowsProc(HWND window, LPARAM);

}  // namespace eef


namespace eef {

void DestroyWindowState(ExplorerWindow& window)
{
    if (IsWindow(window.checkbox)) {
        DestroyWindow(window.checkbox);
    }

    if (IsWindow(window.emptyView)) {
        DestroyWindow(window.emptyView);
    }
}

void LayoutWindowState(ExplorerWindow& window)
{
    if (!IsWindow(window.frame)) {
        return;
    }

    if (!IsWindowVisible(window.frame) || IsIconic(window.frame)) {
        ShowWindow(window.checkbox, SW_HIDE);
        ShowWindow(window.emptyView, SW_HIDE);
        return;
    }

    RECT clientRect;
    if (!GetClientRect(window.frame, &clientRect)) {
        return;
    }

    const int checkboxMargin = ScaleForWindow(window.frame, 18);
    const int checkboxWidth = ScaleForWindow(window.frame, 214);
    const int checkboxHeight = ScaleForWindow(window.frame, 34);
    const int checkboxTop = ScaleForWindow(window.frame, 176);
    POINT checkboxPoint = {
        std::max(checkboxMargin,
                 static_cast<int>(clientRect.right) - checkboxWidth -
                     checkboxMargin),
        std::min(std::max(checkboxMargin, checkboxTop),
                 std::max(checkboxMargin,
                          static_cast<int>(clientRect.bottom) -
                              checkboxHeight - checkboxMargin)),
    };

    if (!ClientToScreen(window.frame, &checkboxPoint)) {
        return;
    }

    int overlayLeft = ScaleForWindow(window.frame, 435);
    int overlayTop = ScaleForWindow(window.frame, 160);

    if (overlayLeft > clientRect.right - ScaleForWindow(window.frame, 120)) {
        overlayLeft = clientRect.right / 3;
    }

    if (overlayTop > clientRect.bottom - ScaleForWindow(window.frame, 120)) {
        overlayTop = clientRect.bottom / 3;
    }

    overlayLeft = std::max(0, overlayLeft);
    overlayTop = std::max(0, overlayTop);

    const int overlayWidth = static_cast<int>(clientRect.right) - overlayLeft;
    const int overlayHeight = static_cast<int>(clientRect.bottom) - overlayTop;
    const bool enabled = g_enabled.load();
    SetWindowPos(window.emptyView,
                 HWND_TOP,
                 overlayLeft,
                 overlayTop,
                 std::max(0, overlayWidth),
                 std::max(0, overlayHeight),
                 SWP_NOACTIVATE | (enabled ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

    SetWindowPos(window.checkbox,
                 HWND_TOP,
                 checkboxPoint.x,
                 checkboxPoint.y,
                 checkboxWidth,
                 checkboxHeight,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

void AddExplorerWindow(HWND frame)
{
    if (FindWindowState(frame)) {
        return;
    }

    HWND checkbox = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                    kToggleClassName,
                                    L"Expandable folders",
                                    WS_POPUP | WS_CLIPSIBLINGS,
                                    0,
                                    0,
                                    0,
                                    0,
                                    frame,
                                    nullptr,
                                    GetModuleHandleW(nullptr),
                                    nullptr);
    if (!checkbox) {
        Wh_Log(L"Failed to create Explorer toggle for window %p", frame);
        return;
    }

    HWND emptyView = CreateWindowExW(WS_EX_NOPARENTNOTIFY,
                                     kOverlayClassName,
                                     L"",
                                     WS_CHILD | WS_CLIPSIBLINGS,
                                     0,
                                     0,
                                     0,
                                     0,
                                     frame,
                                     nullptr,
                                     GetModuleHandleW(nullptr),
                                     nullptr);
    if (!emptyView) {
        DestroyWindow(checkbox);
        Wh_Log(L"Failed to create empty Explorer view for window %p", frame);
        return;
    }

    g_windows.push_back({
        frame,
        checkbox,
        emptyView,
        g_enabled.load(),
    });

    LayoutWindowState(g_windows.back());
    Wh_Log(L"Attached Explorer scaffold controls to window %p", frame);
}

BOOL CALLBACK EnumExplorerWindowsProc(HWND window, LPARAM)
{
    if (IsExplorerFrame(window)) {
        AddExplorerWindow(window);
    }

    return TRUE;
}

void RemoveClosedWindows()
{
    auto newEnd = std::remove_if(
        g_windows.begin(), g_windows.end(), [](ExplorerWindow& window) {
            if (IsWindow(window.frame)) {
                return false;
            }

            DestroyWindowState(window);
            return true;
        });

    g_windows.erase(newEnd, g_windows.end());
}

void SyncCheckboxState()
{
    const bool enabled = g_enabled.load();
    for (ExplorerWindow& window : g_windows) {
        if (!IsWindow(window.checkbox)) {
            continue;
        }

        if (enabled == window.lastChecked) {
            continue;
        }

        window.lastChecked = enabled;
        InvalidateRect(window.checkbox, nullptr, FALSE);
    }
}

}  // namespace eef



namespace eef {

DWORD WINAPI ManagerThreadProc(void*);

}  // namespace eef


namespace eef {

DWORD WINAPI ManagerThreadProc(void*)
{
    RegisterEmptyViewClass();
    RegisterToggleControlClass();

    while (g_running.load()) {
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        RemoveClosedWindows();
        EnumWindows(EnumExplorerWindowsProc, 0);
        SyncCheckboxState();

        for (ExplorerWindow& window : g_windows) {
            LayoutWindowState(window);
        }

        Sleep(100);
    }

    for (ExplorerWindow& window : g_windows) {
        DestroyWindowState(window);
    }

    g_windows.clear();
    UnregisterClassW(kToggleClassName, GetModuleHandleW(nullptr));
    UnregisterClassW(kOverlayClassName, GetModuleHandleW(nullptr));
    return 0;
}

}  // namespace eef


BOOL Wh_ModInit()
{
    eef::g_enabled.store(Wh_GetIntValue(eef::kEnabledValueName, 0) != 0);
    eef::g_running.store(true);

    eef::g_managerThread =
        CreateThread(nullptr, 0, eef::ManagerThreadProc, nullptr, 0, nullptr);
    if (!eef::g_managerThread) {
        eef::g_running.store(false);
        Wh_Log(L"Failed to start Explorer scaffold manager thread");
        return FALSE;
    }

    Wh_Log(L"Explorer Expandable Folders initialized: explorer toggle=%d",
           eef::g_enabled.load());
    return TRUE;
}

void Wh_ModUninit()
{
    eef::g_running.store(false);

    if (eef::g_managerThread) {
        WaitForSingleObject(eef::g_managerThread, 3000);
        CloseHandle(eef::g_managerThread);
        eef::g_managerThread = nullptr;
    }

    Wh_Log(L"Explorer Expandable Folders uninitialized");
}
