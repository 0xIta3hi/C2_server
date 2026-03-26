#include "../include/rpcengine.h"
#include <iostream>

int main(){
    // Generate the C2 payload
    std::string taskRequest = Rpcengine::buildTaskQueryReq();
    
    // Print it to the terminal
    std::cout << "Task Query Payload: " << taskRequest << std::endl;
    
    return 0;
}