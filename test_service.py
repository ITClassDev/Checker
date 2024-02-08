import requests
import json
import sys

payload_test = {
    "language": "cpp",
    "github_link": "https://github.com/ret7020/CppHeaders.git",
    "test_type": "header_test",
    "tests_description": [
        {"name": "itc_abs", "tests": [
            {"input": "-34", "output": "34", "demo": True},
            {"input": "10009", "output": "10009"},
            {"input": "0", "output": "0"},
            {"input": "-2147483647", "output": "2147483647"}   ],
        "submit_id": "65710776-ce95-4f19-8cd9-4b9a1430419f", "types": {"in": ["int"], "out": "int"},
        "env": {"mem": 10, "proc": 2, "time": 2} }
    ]
}

with open(sys.argv[1], "r") as f:
    json_data = f.read()
payload = json.loads(json_data)

result = 0
if(sys.argv[2] == "h"):
    result = requests.post("http://localhost:7777/homework", data=json.dumps(payload), timeout=10000)
if(sys.argv[2] == "c"):
    result = requests.post("http://localhost:7777/challenge", data=json.dumps(payload), timeout=10000)

out_f = open("result.json", "w")
out_f.write(result.text)
out_f.close()
