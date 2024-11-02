/*
 Author: Milind Chabbi
 Date: Jan/8/2022
*/

#include<iostream>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

using namespace std;

uint64_t fib(uint64_t n) {
	if (n < 2) {
		return n;
	}
	uint64_t l = fib(n-1);
	uint64_t r = fib(n-2);
	return l + r;
}

int main(int argc, char**argv) {
    if (argc != 2) {
        cout << "Usage " << argv[0] << " <positive int>\n";
        return -1;
    }
    int numEls = atol(argv[1]);
    if (numEls == 0) {
        cout << "Usage " << argv[0] << " <positive int>\n";
        return -1;
    }
    uint64_t v = fib(numEls);
    cout << v << "\n";
    return 0;
}





