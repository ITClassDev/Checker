# Мысли про реализации
FrontEnd -> [SOLUTION] -> BackEnd -> Checker </br>
Задача в чекере. Что чекер знает? </br>

<b>Одиночная задача:</b></br>
main.cpp или через параметр в json - код решения </br>
tests.json - json с тестами </br>
env.json - json c параметрами <br>

tests.json = [ </br>
    {"input": "1 0 0 0\n10 0 0 0", "output": "91"}  </br>
] </br>
env.json = [ </br>
    {"compiler": "g" or "py", "limit": "4", "memory":"64", "code":" #include <iostream> 
                                                                    int main(){    
                                                                        return 0; 
                                                                    } "                 }</br>
]</br>

Результат работы:</br>
results.json = [</br>
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 0 0 1259 123", "error": "" or "could not find definition for X ..."}</br>
]</br>

<b>Задачи с .h файлами:</b></br>

К каждой функции идут тесты, json имя совпадает с названием функции</br>

itc_len.json = [ </br>
    {"input": "1 0 0 0\n10 0 0 0", "output": "91"}  </br>
] </br>
itc_word.json = [ </br>
    {"input": "1 6 25 0\n10 0 0 6", "output": "25"}  </br>
] </br>
...

Файлы подгружаются из github репозитория при указании ссылки</br>
env.json = [ </br>
    {"compiler": "g" or "py", "limit": "4", "memory":"64", "code":"" ,"link":"https://github.com/ITClassDev/Checker/edit/master/README.md"}</br>
]</br>
