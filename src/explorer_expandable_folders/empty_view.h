#pragma once

#include "common.h"

namespace eef {

LRESULT CALLBACK EmptyViewWndProc(HWND window,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);
void RegisterEmptyViewClass();

}  // namespace eef
