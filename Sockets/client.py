import socket

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("localhost", 8888))
client.sendall(b"Hello, world!")
data = client.recv(2000)
print(data)