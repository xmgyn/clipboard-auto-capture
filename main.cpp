#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <tchar.h>
#include "clipboard.cpp"

LRESULT CALLBACK ClipboardProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLIPBOARDUPDATE) { 
        Clipboard();
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = ClipboardProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("ClipboardMonitor");

    if (!RegisterClass(&wc)) {
        //"Failed to register window class." << std::endl;
        return 0;
    }

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Clipboard Monitor"), 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        // "Failed to create window." << std::endl;
        return 0;
    }

    if (!AddClipboardFormatListener(hwnd)) {
        //"Failed to add clipboard format listener." << std::endl;
        DestroyWindow(hwnd);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveClipboardFormatListener(hwnd);
    DestroyWindow(hwnd);

    return 0;
}