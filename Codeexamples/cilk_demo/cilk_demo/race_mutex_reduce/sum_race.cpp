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
#include <mutex>

#ifdef SYNC
#define LOCK m.lock()
#define UNLOCK m.unlock()
#else
#define LOCK 
#define UNLOCK
#endif
using namespace std;

mutex m;

uint64_t Sum(vector<uint64_t> vec) {
   uint64_t sum = 0;
   cilk_for(int i = 0; i < vec.size(); i++) {
      LOCK;
      uint64_t x = sum;
      sum = x + vec[i];
      UNLOCK;
   }
   return sum;
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
    vector<uint64_t> vec(numEls);
    for(uint64_t i = 0; i < numEls; i++) {
	vec[i] = i;
    }
    cout << Sum(vec) << "\n";
    return 0;
}





