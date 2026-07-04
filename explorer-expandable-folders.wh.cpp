// ==WindhawkMod==
// @id              explorer-expandable-folders
// @name            Explorer Expandable Folders
// @description     Explorer-hosted scaffold for expandable folders.
// @version         0.2.0
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

namespace {

constexpr PCWSTR kOverlayClassName =
    L"ExplorerExpandableFoldersEmptyView";
constexpr PCWSTR kEnabledValueName = L"explorerViewEnabled";
constexpr UINT_PTR kCheckboxId = 1001;

struct ExplorerWindow {
    HWND frame;
    HWND checkbox;
    HWND emptyView;
    bool lastChecked;
};

std::atomic_bool g_running = false;
std::atomic_bool g_enabled = false;
HANDLE g_managerThread = nullptr;
std::vector<ExplorerWindow> g_windows;

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

    RECT clientRect;
    if (!GetClientRect(window.frame, &clientRect)) {
        return;
    }

    const int checkboxX = ScaleForWindow(window.frame, 16);
    const int checkboxY = ScaleForWindow(window.frame, 104);
    const int checkboxWidth = ScaleForWindow(window.frame, 190);
    const int checkboxHeight = ScaleForWindow(window.frame, 28);

    SetWindowPos(window.checkbox,
                 HWND_TOP,
                 checkboxX,
                 checkboxY,
                 checkboxWidth,
                 checkboxHeight,
                 SWP_NOACTIVATE);

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
                 0,
                 0,
                 0,
                 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void AddExplorerWindow(HWND frame)
{
    if (FindWindowState(frame)) {
        return;
    }

    HWND checkbox = CreateWindowExW(WS_EX_NOPARENTNOTIFY,
                                    L"BUTTON",
                                    L"Expandable folders",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                        BS_AUTOCHECKBOX,
                                    0,
                                    0,
                                    0,
                                    0,
                                    frame,
                                    reinterpret_cast<HMENU>(kCheckboxId),
                                    GetModuleHandleW(nullptr),
                                    nullptr);
    if (!checkbox) {
        Wh_Log(L"Failed to create Explorer checkbox for window %p", frame);
        return;
    }

    SendMessageW(checkbox,
                 WM_SETFONT,
                 reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),
                 TRUE);
    SendMessageW(checkbox,
                 BM_SETCHECK,
                 g_enabled.load() ? BST_CHECKED : BST_UNCHECKED,
                 0);

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
    for (ExplorerWindow& window : g_windows) {
        if (!IsWindow(window.checkbox)) {
            continue;
        }

        const bool checked =
            SendMessageW(window.checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
        if (checked == window.lastChecked) {
            continue;
        }

        window.lastChecked = checked;
        g_enabled.store(checked);
        Wh_SetIntValue(kEnabledValueName, checked ? 1 : 0);

        Wh_Log(L"Explorer checkbox changed: enabled=%d", checked);

        for (ExplorerWindow& syncedWindow : g_windows) {
            if (IsWindow(syncedWindow.checkbox)) {
                SendMessageW(syncedWindow.checkbox,
                             BM_SETCHECK,
                             checked ? BST_CHECKED : BST_UNCHECKED,
                             0);
                syncedWindow.lastChecked = checked;
            }
        }
    }
}

DWORD WINAPI ManagerThreadProc(void*)
{
    RegisterEmptyViewClass();

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
    UnregisterClassW(kOverlayClassName, GetModuleHandleW(nullptr));
    return 0;
}

}  // namespace

BOOL Wh_ModInit()
{
    g_enabled.store(Wh_GetIntValue(kEnabledValueName, 0) != 0);
    g_running.store(true);

    g_managerThread =
        CreateThread(nullptr, 0, ManagerThreadProc, nullptr, 0, nullptr);
    if (!g_managerThread) {
        g_running.store(false);
        Wh_Log(L"Failed to start Explorer scaffold manager thread");
        return FALSE;
    }

    Wh_Log(L"Explorer Expandable Folders initialized: explorer checkbox=%d",
           g_enabled.load());
    return TRUE;
}

void Wh_ModUninit()
{
    g_running.store(false);

    if (g_managerThread) {
        WaitForSingleObject(g_managerThread, 3000);
        CloseHandle(g_managerThread);
        g_managerThread = nullptr;
    }

    Wh_Log(L"Explorer Expandable Folders uninitialized");
}
