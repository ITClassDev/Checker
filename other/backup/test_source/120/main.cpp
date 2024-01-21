#include <iostream>

int main(){
	int a,b,c;
	std::cin >> a;
	std::cin >> b;
	std::cin >> c;
	
	if(b == 50){
		volatile int *ptr = (int*)0;
		*ptr = 0;
	}
	else{
	std::cout << (a + b + c) << "\n" << 1;
	}
}
