#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <tchar.h>
#include "clipboard.cpp"

inline LRESULT CALLBACK ClipboardProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLIPBOARDUPDATE) Clipboard();
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

inline void ClipboardMonitor() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = ClipboardProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("ClipboardMonitor");

    if (!RegisterClass(&wc)) {
        std::cerr << "Failed to register window class." << std::endl;
        return;
    }

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Clipboard Monitor"), 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        std::cerr << "Failed to create window." << std::endl;
        return;
    }

    if (!AddClipboardFormatListener(hwnd)) {
        std::cerr << "Failed to add clipboard format listener." << std::endl;
        DestroyWindow(hwnd);
        return;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveClipboardFormatListener(hwnd);
    DestroyWindow(hwnd);
}