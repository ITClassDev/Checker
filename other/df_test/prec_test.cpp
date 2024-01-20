#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdio>

int main(){
	//int p;
	//std::cin >> p;
	double a = std::numeric_limits<double>::max();
	double b = 0.7777777777777777777777777777777777777777777;
	//std::cout << std::fixed << std::setprecision(p) << a << "\n" << std::numeric_limits<long double>::digits10 + 1 << "\n" << (1.0/(p*10)) << "\n";
	std::cout << std::setprecision(19) << a << "\n" << b << "\n";
	std::cout << std::fixed << std::setprecision(20) << a << "\n" << b << "\n";
	//printf("%f\n", a);
	double d = 2.0/3			   ;
	double c = 0.6666666666666666660;
	std::cout << "\n" << (d == c) << "\n" << d << " " << c << "\n";
	double e = 0.0;
	std::cout << "\n" << std::fixed << std::setprecision(5) << e << "\n";  
}
