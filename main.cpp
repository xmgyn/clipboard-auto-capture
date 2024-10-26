#include <windows.h>
#include <iostream>

int main() {
    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return 1;
    }

    // Get clipboard data
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        std::cerr << "Failed to get clipboard data." << std::endl;
        CloseClipboard();
        return 1;
    }

    // Lock the clipboard data
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == NULL) {
        std::cerr << "Failed to lock clipboard data." << std::endl;
        CloseClipboard();
        return 1;
    }

    // Print clipboard data
    std::cout << "Clipboard Data: " << pszText << std::endl;

    // Unlock the clipboard data
    GlobalUnlock(hData);

    // Close the clipboard
    CloseClipboard();

    return 0;
}
