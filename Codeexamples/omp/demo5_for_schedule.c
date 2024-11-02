#include<omp.h>
#include <stdio.h>
#include <stdlib.h>

int nthreads;
int main (int argc, char *argv[]) 
{
int i,tid, nthreads;
omp_set_num_threads(4);
#pragma omp parallel shared(nthreads) 
  {
	#pragma omp for private(tid) schedule(runtime)
	for (i=0; i<16; i++) {
	  tid = omp_get_thread_num();
	  printf("tid= %d i=%d\n", tid,i);
	}
  } 
}
