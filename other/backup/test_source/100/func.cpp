#include "header.h"

int sum(int a, int b){
	if(a == 20){
		volatile int *ptr = (int*)0;
		*ptr = 0;
	}
	return a + b;
}

std::string concat(std::string a, std::string b){
	return a + b;
}

std::vector<char> str_list(std::string str){
	std::vector<char> output;
	for(long long i = 0; i < str.size(); ++i)
		output.push_back(str[i]);
	return output;
}
