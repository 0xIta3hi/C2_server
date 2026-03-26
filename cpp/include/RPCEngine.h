#pragma once
#include <string>

class Rpcengine {
public:
    // Renamed to specifically target our Hub's task slot
    static std::string buildTaskQueryReq();
    static std::string extractResults(const std::string& jsonResponse);
};