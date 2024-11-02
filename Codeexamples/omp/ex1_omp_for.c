#include<omp.h>
#include <stdio.h>
#include <stdlib.h>

int nthreads;
int main (int argc, char *argv[]) 
{
int i,tid, nthreads;

/*** Spawn parallel region ***/
#pragma omp parallel shared(nthreads)
  {
	//Question1: How many times the following print statement executes? 
#pragma omp single
	  {
		  nthreads=omp_get_num_threads();
	printf("Number of threads %d\n",nthreads);
	  }
	//Question2: How many times the following print statement executes? 
	#pragma omp for private(tid)
	for (i=0; i<nthreads; i++) {
	  tid = omp_get_thread_num();
	  printf("tid= %d i=%d\n", tid,i);
	}
  } /*** End of parallel region ***/
}
