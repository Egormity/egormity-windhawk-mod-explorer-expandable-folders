#pragma once

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
