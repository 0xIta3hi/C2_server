#include "../include/RpcEngine.h"
#include <iostream>

int main(){
    std::string blockRequest = RPCEngine::buildBlockNumberReq();
    std::cout << "Block reqest:" << blockRequest << std::endl;
}
