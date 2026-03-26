#include "../include/HTTPClient.h"
#include <iostream>

std::string HTTPClient::postRequest(const std::wstring& host, const std::wstring& path, const std::string& payload) {
    std::string responseData = "";
    
    // 1. Initialize the WinHTTP Session (Pretending to be a generic Windows service)
    HINTERNET hSession = WinHttpOpen(L"Windows-Update-Agent/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "ERROR_SESSION";

    // 2. Connect to the Host (Alchemy)
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return "ERROR_CONNECT"; }

    // 3. Open the POST Request (Using TLS/HTTPS)
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return "ERROR_REQUEST"; }

    // 4. Send the Request with our JSON Payload
    std::wstring headers = L"Content-Type: application/json\r\n";
    BOOL bResults = WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), (LPVOID)payload.c_str(), payload.length(), payload.length(), 0);

    // 5. Receive the Response
    if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

    // 6. Read the Data from the socket
    if (bResults) {
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        do {
            WinHttpQueryDataAvailable(hRequest, &dwSize);
            if (dwSize == 0) break;

            char* pszOutBuffer = new char[dwSize + 1];
            ZeroMemory(pszOutBuffer, dwSize + 1);

            if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                responseData += pszOutBuffer;
            }
            delete[] pszOutBuffer;
        } while (dwSize > 0);
    } else {
        responseData = "ERROR_SEND_RECEIVE";
    }

    // 7. Clean up memory to avoid leaks
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return responseData;
}