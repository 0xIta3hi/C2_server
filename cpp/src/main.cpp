#include "../include/RpcEngine.h"
#include <iostream>

int main(){
    std::string blockRequest = RPCEngine::buildBlockNumberReq();
    std::cout << "Block request: " << blockRequest << std::endl;
    return 0;
}

