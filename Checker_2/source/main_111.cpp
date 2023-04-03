#include <iostream>
#include <string.h>
#include "header.h"
using namespace std;
int main(){
    string a0;
    std::cin >> a0;
    string a1;
    std::cin >> a1;
    std::cout << concat(a0, a1);
}