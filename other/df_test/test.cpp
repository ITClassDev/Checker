#include <iostream>

void kol(int& a, int& b){
	a = 3;
	b = 8;
}

int main(){
	int a = 5;
	int b = 2;
	std::cout << a << " " << b << "\n";
	kol(a,b);
	std::cout << a << " " << b << "\n";
}
