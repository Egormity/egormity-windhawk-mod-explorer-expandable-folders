#include "empty_view.h"
#include "explorer_window.h"
#include "state.h"
#include "toggle_control.h"
#include "window_utils.h"

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
