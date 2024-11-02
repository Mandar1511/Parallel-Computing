//Compare this code with the th_mutex.c in pthreads example
#include <stdio.h>
#include <stdlib.h>
#include<omp.h>

#define VECLEN 100000
#define NUMTHRDS 4

int main (int argc, char *argv[])
{
long i;
double *a, *b;
double sum;


omp_set_num_threads(4);

#pragma omp parallel
{
#pragma omp master
	printf("threadnum:%d\n",omp_get_thread_num());
}

/* Assign storage and initialize values */
a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
  
for (i=0; i<VECLEN*NUMTHRDS; i++) {
  a[i]=1;
  b[i]=a[i];
  }
#pragma omp parallel
{
#pragma omp for reduction(+: sum)
	for(i=0;i<VECLEN*NUMTHRDS;i++){
		sum += a[i]*b[i];
	}
}

printf ("Sum =  %f \n", sum);
free (a);
free (b);
}   
