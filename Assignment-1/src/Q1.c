//Q1. Hello World Program

#include <stdio.h>
#include <omp.h>

int main()
{
	int num_threads;
	
	//Code snippet that is run in parallel by default number of threads
	#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		printf("Hello World from thread with ID %d\n", ID);

		//Executed by one of the threads to obtain the default number of threads
		#pragma omp single
		{
			int nthreads=omp_get_num_threads();
			num_threads=nthreads;
		}
	}
	printf("Default number of threads is %d\n",num_threads);
}