import socket 
import threading
import time

def simulate_client(server_ip, port, password, nickname, message_count, channel="#test"):
    try:
        # Connect to the IRC server
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((server_ip, port))

        # Register the client with PASS, NICK and USER
        sock.sendall(f"PASS {password}\r\n".encode())
        sock.sendall(f"NICK {nickname}\r\n".encode())
        sock.sendall(f"USER {nickname} 0 * :Test Client\r\n".encode())

        # Join the channel
        sock.sendall(f"JOIN {channel}\r\n".encode())

        # Send messages
        for i in range(message_count):
            message = f"Mesage {i} from {nickname}"
            sock.sendall(f"PRIVMSG {channel} :{message}\r\n".encode())
            time.sleep(1) # Delay to avoid spamming to fast
        
        sock.close()
    except Exception as e:
        print(f"Error with clien {nickname}: {e}")

def main():
    server_ip = "127.0.0.1"
    port = 6667
    num_clients = 50
    messages_per_client = 20
    password = "pass"
    threads = []

    # Active multiple clients
    for i in range(num_clients):
        nickname = f"User{i}"
        t = threading.Thread(target=simulate_client, args=(server_ip, port, password, nickname, messages_per_client))
        threads.append(t)
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

if __name__ == "__main__":
    main()