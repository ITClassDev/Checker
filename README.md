# Мысли про реализации
FrontEnd -> [SOLUTION] -> BackEnd -> Checker </br>
Задача в чекере. Что чекер знает? </br>

Одиночная задача: </br>
main.cpp или через параметр в json - код решения </br>
tests.json - json с тестами </br>
env.json - json c параметрами <br>

tests.json = [ </br>
    {"input": "1 0 0 0\n10 0 0 0", "output": "91"}  </br>
] </br>
env.json = [ </br>
    {"compiler": "g" or "py", "limit": "4", "memory":"64", "code":" #include <iostream> </br>
                                                                    int main(){    </br> 
                                                                        return 0; </br>
                                                                    } "                 }</br>
]</br>

Результат работы:
results.json = [
        {"result":"OK" or "WA" or "RT" or "PE" or "CE", "output": " 0 0 0 1259 123", "error": "" or "could not find definition for X ..."}
]
