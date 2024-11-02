//using Monte-Carlo approach: this code is buggy
#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include<cmath>
using namespace std;

int main()
{
	int N = 10000000; // iterating for 10 million times
	int count=0, i; //count is used to count the number of points lying within circle.
	double x, y, rad, pi; //rad holds the radius of an imaginary circle with x and y on the circumference of the circle.  
	int numThreads=1; //run the code with 4 threads
	#pragma omp parallel firstprivate(x, y, rad, i) shared(count) num_threads(numThreads) 
    	{
        	srandom((int)time(NULL) ^ omp_get_thread_num());    //srandom is the function used for initialization/setup of random number generator.
		//Each thread performs N trials.
		for (i=0; i<N; ++i)              
		{
		    x = (double)random()/RAND_MAX;      //random is the function used to generate a random number
		    y = (double)random()/RAND_MAX;      
		    rad = sqrt((x*x)+(y*y));          //sqrt is the function to compute square-root of a number.
		    if (rad<=1)
		    {
			++count;            
		    }
		}
    	}

	pi = ((double)count/(double)(N*numThreads)) * 4.0; //N*numthreads is the total number of trials.

	cout<<pi<<endl; 
	return 0;
}
