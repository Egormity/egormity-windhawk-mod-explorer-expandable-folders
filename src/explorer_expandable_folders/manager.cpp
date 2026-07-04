#include "empty_view.h"
#include "explorer_window.h"
#include "manager.h"
#include "state.h"

namespace eef {

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

}  // namespace eef
