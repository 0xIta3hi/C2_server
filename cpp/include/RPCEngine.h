#pragma once
#include <string>

class Rpcengine {
public:
    static std::string buildBlockNumberReq();
    static std::string extractResults(const std::string& JsonResponse);
};

