#pragma once

#include "common.h"

namespace eef {

void DestroyWindowState(ExplorerWindow& window);
void LayoutWindowState(ExplorerWindow& window);
void AddExplorerWindow(HWND frame);
void RemoveClosedWindows();
void SyncCheckboxState();
BOOL CALLBACK EnumExplorerWindowsProc(HWND window, LPARAM);

}  // namespace eef
