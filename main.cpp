#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

// Function to get clipboard formats
std::vector<UINT> GetClipboardFormats() {
    std::vector<UINT> formats;
    UINT format = 0;

    // Get the first available clipboard format
    format = EnumClipboardFormats(0);
    while (format != 0) {
        formats.push_back(format);
        // Get the next available clipboard format
        format = EnumClipboardFormats(format);
    }

    return formats;
}

// Function to get the name of the clipboard format
std::wstring GetClipboardFormatNameW(UINT format) {
    wchar_t formatName[100];
    if (GetClipboardFormatName(format, formatName, sizeof(formatName) / sizeof(wchar_t))) {
        return std::wstring(formatName);
    }
    else {
        // If it's a predefined format, return the format ID as a string
        return std::to_wstring(format);
    }
}

void CaptureClipboardData() {
    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return;
    }

    // Get clipboard formats
    std::vector<UINT> formats = GetClipboardFormats();

    // Iterate over formats
    for (UINT format : formats) {
        std::wcout << L"Clipboard Format: " << GetClipboardFormatNameW(format) << std::endl;

        HANDLE hData = GetClipboardData(format);
        if (hData != NULL) {
            // Process the clipboard data based on the format
            if (format == CF_TEXT) {
                char* text = static_cast<char*>(GlobalLock(hData));
                if (text != NULL) {
                    std::cout << "Text Data: " << text << std::endl;
                    GlobalUnlock(hData);
                }
            }
            else if (format == CF_BITMAP) {
                // Handle image data
                // (Add your image handling code here)
                std::cout << "Bitmap Data: (image processing logic needed)" << std::endl;
            }
            // Add other formats as needed
        }
    }

    // Close the clipboard
    CloseClipboard();
}

int main() {
    CaptureClipboardData();
    return 0;
}
