import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("0.0.0.0", 6969))
s.listen(1)

while True:
    conn, addr = s.accept()
    print(f"Connection from {addr}")
    while True:
        data = conn.recv(1024)
        if not data: break
        print(data.decode("utf-8", "ignore"), end="")
    conn.close()