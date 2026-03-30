# Decentralized Web3 Command & Control (dC2)

## Overview
dC2 is an asynchronous, decentralized Command and Control (C2) infrastructure that leverages the Ethereum Virtual Machine (EVM) state for tasking and JSON-RPC `eth_call` smuggling for data exfiltration. The architecture eliminates the need for centralized C2 servers by utilizing enterprise Web3 infrastructure (Alchemy) as a high-reputation domain front.

The execution implant is written in native C++ utilizing the Win32 API for evasive Inter-Process Communication (IPC) and memory-safe process execution.

## Architecture

The framework consists of three primary components:

1.  **EVM Smart Contract (`Hub.sol`):** Acts as the immutable tasking router. Commands are stored as hex-encoded bytes in the contract's state trie (Storage Slot `0x1`).
2.  **C++ Windows Implant:** A lightweight polling agent that queries the EVM via JSON-RPC over WinHTTP, decodes EVM memory padding, executes commands via Win32 Anonymous Pipes, and exfiltrates `stdout` via gas-less `eth_call` requests.
3.  **Python Operator Terminal:** A `web3.py` script that handles ECDSA transaction signing, gas estimation, and payload broadcasting to the Sepolia testnet.

## Directory Structure
```text
dC2/
├── hub/
│   ├── src/Hub.sol          # Deployed EVM Smart Contract
│   ├── operator.py          # Operator tasking script
│   └── .env                 # Node URLs and ECDSA Private Keys
└── cpp/
    ├── include/
    │   ├── HTTPClient.h     # WinHTTP definitions
    │   └── rpcengine.h      # JSON-RPC payload constructors
    └── src/
        ├── main.cpp         # Polling loop, EVM decoder, and execution engine
        ├── HTTPClient.cpp   # WinHTTP implementation
        └── rpcengine.cpp    # JSON-RPC parsing
```


## Technical Implementation

### 1. EVM State Tasking & Decoding

Commands are pushed to the blockchain as hex strings. Because the EVM implements Short String Optimization for `bytes` arrays under 32 bytes, the data is left-aligned and zero-padded.

- The implant queries the storage slot via `eth_getStorageAt`.
    
- The `decodeEVMString` function strips the `0x` prefix and dynamically parses the 32-byte EVM word, breaking the loop upon encountering the `00` zero-padding to prevent passing garbage memory to the Win32 execution engine.
    

### 2. Execution Engine (Win32 IPC)

The implant executes arbitrary commands without relying on the C-runtime `system()` or `popen()` functions to maintain OPSEC (preventing visible `cmd.exe` windows) and to capture `stdout` natively.

- **Pipe Initialization:** Utilizes `CreatePipe` to establish an anonymous IPC channel. `SetHandleInformation` is called with `HANDLE_FLAG_INHERIT = 0` on the Read pipe to prevent deadlocks caused by the child process inheriting the read handle.
    
- **Process Creation:** `STARTUPINFOA` is configured with `STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW` and `SW_HIDE`. The `hStdOutput` and `hStdError` handles are routed to the Write end of the anonymous pipe.
    
- **Memory Safety:** The command string is copied into a mutable `std::vector<char>` buffer before being passed to `CreateProcessA`, preventing Access Violations if the OS attempts to modify the command line argument string.
    
- **Data Extraction:** The Write handle is closed in the parent process, and a `ReadFile` loop blocks until the pipe buffer is drained, appending the raw bytes into a C++ `std::string`.
    

### 3. Exfiltration (`eth_call` Smuggling)

To exfiltrate data without packaging ECDSA signing libraries (e.g., `libsecp256k1`) into the C++ binary or paying transaction gas, the implant utilizes `eth_call` smuggling.

- The `stdout` string is converted to a continuous hex string.
    
- The implant wraps the hex data into the `"data"` parameter of an `eth_call` JSON-RPC request targeting the smart contract.
    
- The EVM reverts the call (as the data does not match a valid function selector), but the Web3 RPC Gateway (Alchemy) logs the full request payload.
    
- Operators retrieve the exfiltrated `stdout` by monitoring the Alchemy RPC logs.
    

## Setup & Usage

### Prerequisites

- Foundry (for smart contract deployment)
    
- Python 3.11+ (for Operator Terminal)
    
- MSVC / MinGW (for compiling the C++ implant)
    
- Alchemy Account (RPC Node)
    

### 1. Configure Environment

Populate the `dC2/hub/.env` file:

Code snippet

```
SEPOLIA_RPC_URL="[https://eth-sepolia.g.alchemy.com/v2/](https://eth-sepolia.g.alchemy.com/v2/)<YOUR_API_KEY>"
ALCHEMY_API_KEY="<YOUR_API_KEY>"
PRIVATE_KEY="<YOUR_ECDSA_PRIVATE_KEY>"
```

### 2. Deploy Smart Contract

Bash

```
cd hub
forge create src/Hub.sol:Hub --rpc-url sepolia --private-key <PRIVATE_KEY>
```

_Update `CONTRACT_ADDRESS` in `operator.py` and `rpcengine.cpp` with the deployed address._

### 3. Compile and Run Implant

DOS

```
cd cpp
g++ src/*.cpp -I include -lwinhttp -o poller.exe
.\poller.exe
```

### 4. Task the Implant

From an isolated Python virtual environment:

DOS

```
cd hub
python -m venv venv
.\venv\Scripts\activate
pip install web3 python-dotenv
python operator.py "whoami"
```

Retrieve the output by monitoring the `eth_call` JSON payloads in your Alchemy Dashboard logs.