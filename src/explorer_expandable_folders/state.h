#pragma once

#include "common.h"

namespace eef {

extern std::atomic_bool g_running;
extern std::atomic_bool g_enabled;
extern HANDLE g_managerThread;
extern std::vector<ExplorerWindow> g_windows;

}  // namespace eef
