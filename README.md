# Мысли про реализации
FrontEnd -> [SOLUTION] -> BackEnd -> Checker </br>
Задача в чекере. Что чекер знает? </br>

<b>Одиночная задача:</b></br>
main.cpp или через параметр в json - код решения </br>
tests.json - json с тестами </br>
env.json - json c параметрами <br>

tests.json = [ </br>
    {"input": "1 0 0 0\n10 0 0 0", "output": "91"}</br>
    {"input": "1 97 0\n450 10 87", "output": "124"}</br>
    {"input": "1 345 0 0\n10 6423 0 234", "output": "90015"}</br>
] </br>
env.json = [ </br>
    {"compiler": "g" or "py", "limit": "4", "memory":"64", "code":" #include <iostream> 
                                                                    int main(){    
                                                                        return 0; 
                                                                    } "                 }</br>
]</br>

Результат работы:</br>
results.json = [</br>
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 0 0 1259 123", "error": "could not find definition for X ..."}</br>
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 1234 0 12 97", "error": ""}</br>
]</br>

<b>Задачи с .h файлами:</b></br>

К каждой функции идут тесты,  имя списка совпадает с названием функции</br>

tests.json = [ </br>
   {"itc_len": [{"input": "1 0 0 0\n10 0 0 0", "output": "91"}</br>
                {"input": "2 97 0\n450 10 87", "output": "124"}</br>
                {"input": "1 345 0 0\n10 6423 0 234", "output": "90015"}]}</br>
                
   {"itc_max": [{"input": "1234 12345 1234 ", "output": "789"}</br>
                {"input": "8 97 0\n450 10 87", "output": "123"}</br>
                {"input": "1 345  0\n10 123 0 9752", "output": "345"}]}</br>
]</br>

Файлы подгружаются из github репозитория при указании ссылки</br>
env.json = [ </br>
    {"type":"h", "compiler": "g" or "py", "limit": "4", "memory":"64", "code":"" , "link":"https://github.com/ITClassDev/"}</br>
]</br>
