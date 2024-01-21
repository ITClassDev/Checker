#include "header.h"
#include <vector>

using namespace std;

void odd_even(const vector<int> &lst, vector<int> &lst1, vector<int> &lst2){
	if(lst.size() > 0){
		for(long long i = 0; i < lst.size(); ++i){
			if(lst[i] % 2 == 0) lst1.push_back(lst[i]);
			else lst2.push_back(lst[i]);
		}
	}
}
