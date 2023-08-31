# Задачи
прибраться...

# Принцип работы 
FrontEnd -> BackEnd -> Checker </br>

<b>Одиночная задача:</b></br>
tests.json - json с тестами </br>
```
tests.json = {
"tests":[
	["input": "100\n50\n25", "output": "175\n1"],
	["input": "1\n2\n3", "output": "6\n1"],
	["input": "60\n30\n15", "output": "105\n1"]	],
"submit_id": 120, "env": ["time": 2, "memory": 1024]
};
```
<b>Задачи с .h файлами:</b></br>

К каждой функции идут тесты, лимиты, типы входа и выхода </br>
 ```
tests.json = [
{"name":"sum","tests":[
	["input": "100\n120", "output": "220"],
	["input": "20\n43", "output": "63"],
	["input": "7\n123", "output": "130"]	], 
"submit_id": 100, "types": ["in": ["int", "int"], "out": "int"], "env": ["time": 2, "memory": 1024]},
{"name":"str_list","tests":[
	["input": "abcdef", "output": "a b c d e f "],
	["input": "lokira", "output": "l o k i r a "],
	["input": "pgsppl", "output": "p g s p p l "]	], 
"submit_id": 105, "types": ["in": ["string"], "out": "vector<char>"], "env", ["time": 2, "memory": 1024]}
]
```

Результат работы:</br>
 ```
main_test_results.json = [
        {"OOM":false,"duration":"0:0:0.009511","exitcode":"0","status":true},
        {"OOM":false,"duration":"0:0:0.003160","exitcode":"139","status":false},
]
header_test_results.json = [
    [
        {"OOM":false,"duration":"0:0:0.003521","exitcode":"0","status":true},
        {"OOM":false,"duration":"0:0:0.067150","exitcode":"139","status":false},
        {"OOM":false,"duration":"0:0:0.002409","exitcode":"0","status":true}
    ],
    [
        {"OOM":false,"duration":"0:0:2.003915","exitcode":"137","status":false},
        {"OOM":false,"duration":"0:0:0.001897","exitcode":"0","status":true},
        {"OOM":false,"duration":"0:0:0.002486","exitcode":"0","status":true}
    ],
    [
        {"OOM":false,"duration":"0:0:0.002214","exitcode":"0","status":true},
        {"OOM":false,"duration":"0:0:0.001931","exitcode":"0","status":true},
        {"OOM":true,"duration":"0:0:0.042718","exitcode":"137","status":false}
    ]
]
error_results.json = [
	{"error":1,"error_msg":"error ....."}
]
 ```
Docker attaching
Attach via websockets to process with pid 1:
```
docker run -itd --name=itc_python ubuntu:20.04 bash -i -c "bash -i" # Run container with bash
socat -d -d TCP-L:2375,fork UNIX:/var/run/docker.sock # proxy docker socket to local tcp port 2375
websocat "ws://127.0.0.1:2375/containers/4c45d07e9038/attach/ws?stream=1&stdout=1&stdin=1&logs=1" # run test websocket client to execute bash commands
```
