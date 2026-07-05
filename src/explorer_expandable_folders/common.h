#pragma once

#include <windows.h>
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
#include <vector>

namespace eef {

constexpr PCWSTR kToggleClassName =
    L"ExplorerExpandableFoldersToggle";
constexpr PCWSTR kEnabledValueName = L"explorerViewEnabled";

struct ExplorerWindow {
    HWND frame;
    HWND checkbox;
    HWND nativeView;
    bool lastChecked;
    bool nativeViewHidden;
};

}  // namespace eef
