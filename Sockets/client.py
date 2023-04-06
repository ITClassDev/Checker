import socket
import json
import threading
import os
import random

single_source = '''
a, b = map(int, input().split())
print(a + b)
'''

def listen(s):
    while True:
        data = s.recv(4096)
        if data:
            try:
                data = json.loads(data.decode("utf-8"))
                print("Response:", data)
                s.close()
            except:
                print("Invalid data")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 5208))

payload = {"type": 0, "env": {"time": 20, "memory": 1024, "language": "python"}, "tests": [
    {"input": "100 25", "output": "125"},
    {"input": "1 10", "output": "11"},
    {"input": "60 15", "output": "75"}], "submit_id": 120}

#print(json.dumps(payload))

threading.Thread(target=lambda: listen(s)).start()
# Run new test instance
workspace_id = random.randint(10 ** 3, 10 ** 4)
os.mkdir(f"./workspace/{workspace_id}")
payload["submit_id"] = workspace_id

s.sendall(json.dumps(payload).encode("utf-8"))

