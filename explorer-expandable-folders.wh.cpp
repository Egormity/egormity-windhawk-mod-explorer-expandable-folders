// ==WindhawkMod==
// @id              explorer-expandable-folders
// @name            Explorer Expandable Folders
// @description     Initial Explorer-injectable scaffold for expandable folders.
// @version         0.1.0
// @author          Egormity
// @include         explorer.exe
// ==/WindhawkMod==

#include <windows.h>
#include <windhawk_api.h>

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Expandable Folders initialized");
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Expandable Folders uninitialized");
}
