# Принцип работы 
FrontEnd -> BackEnd -> Checker </br>

<b>Одиночная задача:</b></br>
main.cpp или через параметр в json - код решения </br>
tests.json - json с тестами </br>
env.json - json c параметрами <br>
```
tests.json = {
    "tests":[{"input": "1 0 0 0\n10 0 0 0", "output": "91"}
    {"input": "1 97 0\n450 10 87", "output": "124"}
    {"input": "1 345 0 0\n10 6423 0 234", "output": "90015"}]
}
env.json = [ 
    {"compiler": "g" or "py", "limit": "4", "memory":"64", "code":" #include <iostream> 
                                                                    int main(){    
                                                                        return 0; 
                                                                    } "  }
]
```

Результат работы:</br>
 ```
results.json = [
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 0 0 1259 123", "error": "could not find definition for X ..."}
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 1234 0 12 97", "error": ""}
]
 ```
<b>Задачи с .h файлами:</b></br>

К каждой функции идут тесты,  имя списка совпадает с названием функции</br>
 ```
tests.json = [ </br>
   {"itc_len": {"tests": [{"input": "1 0 0 0\n10 0 0 0", "output": "91"}
                {"input": "2 97 0\n450 10 87", "output": "124"}
                {"input": "1 345 0 0\n10 6423 0 234", "output": "90015"}], "arguments": ["string"]}}
                
   {"itc_max": [{"input": "1234 12345 1234 ", "output": "789"}
                {"input": "8 97 0\n450 10 87", "output": "123"}
                {"input": "1 345  0\n10 123 0 9752", "output": "345"}]}
]
```

Файлы подгружаются из github репозитория при указании ссылки</br>
```
env.json = [ 
    {"type":"h", "compiler": "g" or "py", "limit": "4", "memory":"64", "code":"" , "link":"https://github.com/ITClassDev/"}
]
```
Docker attaching
Attach via websockets to process with pid 1:
```
docker run -itd --name=itc_python ubuntu:20.04 bash -i -c "bash -i" # Run container with bash
socat -d -d TCP-L:2375,fork UNIX:/var/run/docker.sock # proxy docker socket to local tcp port 2375
websocat "ws://127.0.0.1:2375/containers/4c45d07e9038/attach/ws?stream=1&stdout=1&stdin=1&logs=1" # run test websocket client to execute bash commands
```
