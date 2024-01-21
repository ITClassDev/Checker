# Принцип работы 
для каждой проверки нужен json

### Одиночная задача:
```
main_test.json = {
    "language": "cpp",
    "github_link": "https://github.com/ikoshkila/main_test_shtp.git",
    "test_type": "main_test",
    "debug": false,
    "tests_description": {
        "tests": [
            {"input": "100\n50\n25", "output": "175\n1"},
            {"input": "1\n2\n3", "output": "6\n1"},
            {"input": "60\n30\n15", "output": "105\n1"}   ],
        "submit_id": "120",
        "env": {"mem": 10, "proc": 2, "time": 2}
    }
}
```
### Задачи с .h файлами:

К каждой функции идут тесты, лимиты, типы входа и выхода </br>
```
header_test.json = {
    "language": "cpp",
    "github_link": "https://github.com/ret7020/CppHeaders.git",
    "test_type": "header_test",
    "debug": false,
    "tests_description": [
        {"name": "itc_name", "tests": [
            {"input": "", "output": "ANY_ANSWER_CORRECT"}   ],
        "submit_id": "8", "types": {"in": ["void"], "out": "void"},
        "env": {"mem": 10, "proc": 2, "time": 2} },
        {"name": "itc_abs", "tests": [
            {"input": "-34", "output": "34"},
            {"input": "10009", "output": "10009"},
            {"input": "0", "output": "0"},
            {"input": "-2147483647", "output": "2147483647"}   ],
        "submit_id": "10", "types": {"in": ["int"], "out": "int"},
        "env": {"mem": 10, "proc": 2, "time": 2} },
        {"name": "itc_fabs", "tests": [
            {"input": "-34.78", "output": "34.00000"},
            {"input": "34.99", "output": "34.00000"},
            {"input": "-3333444234.99", "output": "2147483647.00000"},
            {"input": "0.0", "output": "0.00000"},
            {"input": "-2147483648234324324324456546.345345435345423899", "output": "-2147483647.00000"}   ],
        "submit_id": "11", "types": {"in": ["double"], "out": "double"},
        "env": {"mem": 10, "proc": 2, "time": 2} }
    ]
}
```
### Результат работы:
 ```
main_test_results.json = {
    {"error":0,"solved":true,"submit_id":"120","tests_results":[
        {"OOM":false,"duration":"0:0:0.009511","exitcode":"0","status":true},
        {"OOM":false,"duration":"0:0:0.003160","exitcode":"139","status":false} }
    ]}
}
header_test_results.json = [
    {"error":0,"solved":true,"submit_id":"8","tests_results":[
        {"OOM":false,"duration":"0:0:0.001567","exitcode":"0","passed":true}
    ]},
    {"error":0,"solved":true,"submit_id":"10","tests_results":[
        {"OOM":false,"duration":"0:0:0.002680","exitcode":"0","passed":true},
        {"OOM":false,"duration":"0:0:0.001990","exitcode":"0","passed":true},
        {"OOM":false,"duration":"0:0:0.001830","exitcode":"0","passed":true},
        {"OOM":false,"duration":"0:0:0.002076","exitcode":"0","passed":true}
    ]},
    {"error":0,"solved":false,"submit_id":"11","tests_results":[
        {"OOM":false,"duration":"0:0:0.001694","exitcode":"0","passed":false},
        {"OOM":false,"duration":"0:0:0.001804","exitcode":"0","passed":false},
        {"OOM":false,"duration":"0:0:0.001776","exitcode":"0","passed":false},
        {"OOM":false,"duration":"0:0:0.001648","exitcode":"0","passed":true},
        {"OOM":false,"duration":"0:0:0.009961","exitcode":"0","passed":false}
    ]},
]
error_results.json = [
	{"error":1,"error_msg":"error ....."}
]
```
(больше информации можно найти в info.txt)
# Docker attaching
Attach via websockets to process with pid 1:
```
docker run -itd --name=itc_python ubuntu:20.04 bash -i -c "bash -i" # Run container with bash
socat -d -d TCP-L:2375,fork UNIX:/var/run/docker.sock # proxy docker socket to local tcp port 2375
websocat "ws://127.0.0.1:2375/containers/4c45d07e9038/attach/ws?stream=1&stdout=1&stdin=1&logs=1" # run test websocket client to execute bash commands
```
