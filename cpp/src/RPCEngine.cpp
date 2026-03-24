#include "../include/RpcEngine.h"

std::string RPCEngine::buildBlockNumberReq() {
    // Minified JSON payload to query the latest block number.
    // "id": 1 is arbitrary but required by the JSON-RPC 2.0 specification.
    return "{\"jsonrpc\":\"2.0\",\"method\":\"eth_blockNumber\",\"params\":[],\"id\":1}";
}

std::string RPCEngine::extractResults(const std::string& jsonResponse) {
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