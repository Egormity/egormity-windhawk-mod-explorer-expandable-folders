#pragma once

#include "common.h"

namespace eef {

int ScaleForWindow(HWND window, int value);
bool IsExplorerFrame(HWND window);
ExplorerWindow* FindWindowState(HWND frame);

}  // namespace eef
