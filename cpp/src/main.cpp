#include "../include/rpcengine.h"
#include <iostream>

int main(){
    std::string blockRequest = Rpcengine::buildBlockNumberReq();
    std::cout << "Block request: " << blockRequest << std::endl;
    return 0;
}

