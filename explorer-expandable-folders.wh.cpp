// ==WindhawkMod==
// @id              explorer-expandable-folders
// @name            Explorer Expandable Folders
// @description     Initial Explorer-injectable scaffold for expandable folders.
// @version         0.1.1
// @author          Egormity
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable expandable folders scaffold
  $description: >-
    Visible kill switch for this mod's own behavior. The current scaffold only
    logs its state; future Explorer logic will run only when this is enabled.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>

struct Settings {
    bool enabled;
};

Settings g_settings;

void LoadSettings()
{
    g_settings.enabled = Wh_GetIntSetting(L"enabled");
}

BOOL Wh_ModInit()
{
    LoadSettings();

    Wh_Log(L"Explorer Expandable Folders initialized: enabled=%d",
           g_settings.enabled);
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();

    Wh_Log(L"Explorer Expandable Folders settings changed: enabled=%d",
           g_settings.enabled);
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Expandable Folders uninitialized");
}
