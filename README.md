# Мысли про реализации
FrontEnd -> [SOLUTION] -> BackEnd -> Checker </br>
Задача в чекере. Что чекер знает? </br>
Одиночная задача: </br>
main.cpp - код решения </br>
tests.json - json с тестами </br>
env.json - конфиги для чекера - лимит по памяти, времени и какой компилятор юзать </br>

Пример тестов (Simple): </br>
tests.json = [ </br>
    {"input": "1 0 0 0\n10 0 0 0", "output": "91"}  </br>
] </br>

Так же надо подумать о тестах с генераторами. Где типо чекать ответ по алгоритму. Но это уже потом. </br>