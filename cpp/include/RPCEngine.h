#pragma once
#include <string>

class RPCEngine {
public:
    static std::string buildBlockNumberReq();
    static std::string extractResults(const std::string& JsonResponse);
};

