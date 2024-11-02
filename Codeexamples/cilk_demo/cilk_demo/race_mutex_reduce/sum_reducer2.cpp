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


void identity(void *view) {
        *(uint64_t*)view = 0;
}
void addInt64(void *left, void *right) {
        *(uint64_t*)left += *(uint64_t*)right;
}

uint64_t Sum(vector<uint64_t> vec) {
   uint64_t cilk_reducer(identity, addInt64) sum ;
   sum = 0;

   cilk_for(int i = 0; i < vec.size(); i++) {
      sum += vec[i];
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





