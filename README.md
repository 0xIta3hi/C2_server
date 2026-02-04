# C2 Server

A lightweight Command & Control (C2) server implementation for remote command execution. This project demonstrates a basic client-server architecture for executing system commands on remote machines.

## Overview

This C2 server consists of two main components:
- **Server** (`main.py`): Manages incoming client connections and sends commands
- **Client** (`client.py`): Connects to the server and executes received commands

The server supports multiple simultaneous client connections and provides an interactive shell interface for command execution.

## Features

- 🔌 Multi-client connection management
- 💻 Remote command execution
- 📋 Session enumeration and selection
- 📁 Directory navigation support
- 🔄 Persistent connection handling

## Requirements

- Python 3.6+
- Windows or Linux operating system
- Network connectivity between server and client machines

## Installation

1. Clone or download this repository
2. No external dependencies required (uses Python standard library only)

```bash
git clone <repository-url>
cd C2_server
```

## Usage

### Starting the Server

Run the server component to start listening for client connections:

```bash
python main.py
```

The server will bind to `127.0.0.1:5555` and display:
```
server listening..
```

### Connecting a Client

On the target machine, run the client:

```bash
python client.py
```

The client will attempt to connect to `127.0.0.1:5555` (modify the IP address in the code to point to your server).

### Server Commands

Once the server is running, use these commands:

| Command | Description |
|---------|-------------|
| `list` | Display all active client sessions |
| `select <ID>` | Enter an interactive session with a specific client |
| `exit` | Terminate the server |

### Interactive Session Commands

When you've selected a client session (`select <ID>`), you can execute system commands:

```
Shell(IP.Address)> ipconfig
Shell(IP.Address)> dir
Shell(IP.Address)> cd C:\
Shell(IP.Address)> exit
```

Type `exit` to return to the main C2 prompt.

## Example Workflow

```
C2:/>list
--Active-session--
Enumate: 0:192.168.1.100
Enumate: 1:192.168.1.101

C2:/>select 0
[*] Entered Session 0. Type 'exit' to background.
Shell(192.168.1.100)> whoami
DESKTOP-USER\Administrator

Shell(192.168.1.100)> cd C:\Users
Changed dir to C:\Users

Shell(192.168.1.100)> exit
C2:/>
```

## Project Structure

```
C2_server/
├── main.py          # Server implementation
├── client.py        # Client implementation
└── README.md        # Documentation
```

## How It Works

### Server Operation
1. Creates a TCP socket and binds to port 5555
2. Starts a background thread to accept incoming connections
3. Maintains lists of connected clients and their IP addresses
4. Provides an interactive CLI to manage sessions and send commands

### Client Operation
1. Connects to the server via TCP socket
2. Waits to receive commands from the server
3. Executes commands using subprocess
4. Handles special cases (like `cd` command) for directory navigation
5. Returns command output back to the server

## Configuration

Modify these values in the source files as needed:

**Server** (`main.py`):
```python
server.bind(('127.0.0.1', 5555))  # Change IP and port
```

**Client** (`client.py`):
```python
s.connect(('127.0.0.1', 5555))  # Point to your server IP
```

## Security Considerations

⚠️ **Warning**: This is an educational project for learning purposes only. It demonstrates basic C2 concepts and should not be used for unauthorized access to systems.

### Limitations:
- No encryption or authentication
- Plaintext command transmission
- No error handling for network failures
- Limited command execution capabilities
- Single-threaded server (may struggle with many clients)

For production use, consider implementing:
- TLS/SSL encryption
- Authentication mechanisms
- Robust error handling
- Proper logging and monitoring
- Rate limiting

## Troubleshooting

### Client won't connect
- Verify server is running
- Check IP address and port in client.py
- Ensure firewall allows connections on port 5555

### Commands not executing
- Check for typos in command syntax
- Ensure the command is valid on the target OS
- Verify network connectivity

### Session drops
- Network connectivity issues
- Server crash or restart
- Client intentionally closed connection

## License

This project is for educational purposes only. Use responsibly and legally.

## Disclaimer

This C2 server is provided as-is for learning and authorized security testing only. Unauthorized access to computer systems is illegal. The authors assume no liability for misuse.
