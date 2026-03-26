#pragma once
#include <string>
#include <windows.h>
#include <winhttp.h>

class HTTPClient {
public:
    // Takes the target host, API path, and your JSON payload
    static std::string postRequest(const std::wstring& host, const std::wstring& path, const std::string& payload);
};