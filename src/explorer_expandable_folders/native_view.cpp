#include "native_view.h"

namespace eef {

struct NativeViewSearch {
    HWND frame;
    HWND toggle;
    HWND bestWindow;
    int bestScore;
};

bool GetClassNameEquals(HWND window, PCWSTR className)
{
    wchar_t actualClassName[128] = {};
    GetClassNameW(window, actualClassName,
                  static_cast<int>(ARRAYSIZE(actualClassName)));
    return lstrcmpW(actualClassName, className) == 0;
}

int GetNativeViewClassScore(HWND window)
{
    if (GetClassNameEquals(window, L"SHELLDLL_DefView")) {
        return 60000;
    }

    if (GetClassNameEquals(window, L"SysListView32")) {
        return 50000;
    }

    if (GetClassNameEquals(window, L"DirectUIHWND")) {
        return 25000;
    }

    if (GetClassNameEquals(window, L"UIItemsView")) {
        return 25000;
    }

    return 0;
}

BOOL CALLBACK EnumNativeViewCandidates(HWND window, LPARAM param)
{
    NativeViewSearch& search =
        *reinterpret_cast<NativeViewSearch*>(param);

    if (window == search.toggle || !IsWindowVisible(window)) {
        return TRUE;
    }

    const int classScore = GetNativeViewClassScore(window);
    if (classScore == 0) {
        return TRUE;
    }

    RECT rect;
    RECT clientRect;
    if (!GetWindowRect(window, &rect) ||
        !GetClientRect(search.frame, &clientRect)) {
        return TRUE;
    }

    POINT topLeft = {rect.left, rect.top};
    POINT bottomRight = {rect.right, rect.bottom};
    if (!ScreenToClient(search.frame, &topLeft) ||
        !ScreenToClient(search.frame, &bottomRight)) {
        return TRUE;
    }

    const int width = bottomRight.x - topLeft.x;
    const int height = bottomRight.y - topLeft.y;
    if (width < 160 || height < 100) {
        return TRUE;
    }

    const int areaScore = (width * height) / 1000;
    const int filePaneBias =
        topLeft.x > clientRect.right / 4 ? 10000 : 0;
    const int contentBias =
        topLeft.y > 90 && topLeft.y < clientRect.bottom - 80 ? 5000 : 0;
    const int score = classScore + filePaneBias + contentBias + areaScore;

    if (score > search.bestScore) {
        search.bestScore = score;
        search.bestWindow = window;
    }

    return TRUE;
}

HWND FindNativeExplorerView(HWND frame, HWND toggle)
{
    NativeViewSearch search = {
        frame,
        toggle,
        nullptr,
        0,
    };

    EnumChildWindows(frame, EnumNativeViewCandidates,
                     reinterpret_cast<LPARAM>(&search));
    return search.bestWindow;
}

}  // namespace eef
