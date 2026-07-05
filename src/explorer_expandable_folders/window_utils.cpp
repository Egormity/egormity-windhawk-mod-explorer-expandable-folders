#include "state.h"
#include "window_utils.h"

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
