#include "manager.h"
#include "state.h"

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
