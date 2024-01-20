#include <iostream>
#include <string>

int main(){
	int a;
	std::string b;
	std::string c;
	
	std::cin >> a;
	std::cin.ignore(256, '\n');
	getline(std::cin, b);
	//std::cin.ignore(256, '\n');
	getline(std::cin, c);

	std::cout << a << " ; " << b << " ; " << c << " |\n";

	return 0;
}
