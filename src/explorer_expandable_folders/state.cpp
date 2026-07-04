#include "state.h"

namespace eef {

std::atomic_bool g_running = false;
std::atomic_bool g_enabled = false;
HANDLE g_managerThread = nullptr;
std::vector<ExplorerWindow> g_windows;

}  // namespace eef
