#include "header.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

int main(){
	vector<int> a;
	vector<int> b;
	vector<int> c;

	string a0;
    getline(cin,a0);
	istringstream a1(a0);
	int a2;
	while(a1 >> a2)
		a.push_back(a2);

	odd_even(a, b, c);
    for(auto e:c)
        std::cout << e << " ";
	std::cout << "\n";
	for(auto e:b)
		std::cout << e << " ";
	std::cout << "\n";

	vector<int> d,o,f,g;
	string d0;
	getline(cin, d0);
	istringstream d1(d0);
	int d2;
	while(d1 >> d2)
		d.push_back(d2);
	
	pos_neg(d,o,f,g);
	for(auto e:g)
		std::cout << e << " ";
	std::cout << "\n";
	for(auto e:o)
		std::cout << e << " ";
	std::cout << "\n";
}
