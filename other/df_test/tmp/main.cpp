#include <iostream>

int main(){
	std::string a = "abcdefghi";
	std::cout << a << "\n";
	if(a.find(',') != -1) a.erase(a.find(','), 1);
	std::cout << a << "\n";
	return 0;
}
