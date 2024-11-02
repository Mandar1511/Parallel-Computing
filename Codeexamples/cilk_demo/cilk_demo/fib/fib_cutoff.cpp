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
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

using namespace std;

uint64_t fibSerial(uint64_t n) {
	if (n < 2) {
		return n;
	}
	uint64_t l = fibSerial(n-1);
	uint64_t r = fibSerial(n-2);
	return l + r;
}

uint64_t fib(uint64_t n, int depth, int cutoff) {
        if (depth > cutoff) {
            return fibSerial(n);
        }
	if (n < 2) {
		return n;
	}
        depth += 1;
	uint64_t l = cilk_spawn fib(n-1, depth, cutoff);
	uint64_t r = cilk_spawn fib(n-2, depth, cutoff);
	cilk_sync;
	return l + r;
}

int main(int argc, char**argv) {
    if (argc != 3) {
        cout << "Usage " << argv[0] << " <positive int> <cutoff>\n";
        return -1;
    }
    int numEls = atol(argv[1]);
    if (numEls == 0) {
        cout << "Usage " << argv[0] << " <positive int> <cutoff>\n";
        return -1;
    }
    int cutoff = atol(argv[2]);
    if (numEls == 0) {
        cout << "Usage " << argv[0] << " <positive int> <cutoff>\n";
        return -1;
    }
 
    uint64_t v = fib(numEls, 0, cutoff);
    cout << v << "\n";
    return 0;
}
