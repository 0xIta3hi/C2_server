// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import {Script, console} from "forge-std/Script.sol";
import {Hub} from "../src/Hub.sol";

contract DeployHub is Script {
    function run() external returns (Hub) {
        // Pulls the private key securely from your .env file
        uint256 deployerPrivateKey = vm.envUint("OPERATOR_PRIVATE_KEY");

        // Instructs Foundry to use this key to sign subsequent transactions
        vm.startBroadcast(deployerPrivateKey);

        // Deploys the contract to the network
        Hub hub = new Hub();

        vm.stopBroadcast();

        // Outputs the final memory address so we can feed it to the C++ implant
        console.log("Hub deployed successfully to:", address(hub));

        return hub;
    }
}