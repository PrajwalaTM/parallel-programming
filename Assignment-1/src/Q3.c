//Q3. DAXPY Loop

#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define vector_size pow(2,16)
#define NUM_THREADS 32

int vectorX[1<<16];
int vectorY[1<<16];

//Code to initialize X and Y vectors with random values
void initialize_vectors()
{
	int i;
	for(i=0;i<(int)vector_size;i++)
	{
		//vectorX takes random values between 0 and 100 
		int r1 = rand() % 101;
		vectorX[i] = r1;

		//vectorY takes random values between 0 and 500
		int r2 = rand() % 501;
		vectorY[i] = r2;
	}
}

int main()
{
	int a = 10;
	int j;

	double max_speedup=0,serial_time;
	int max_speedup_thread_num=0;
	int num_threads;

	// Loop that evaluates the performance using 1,2,3...NUM_THREADS threads 
	for(j=1;j<=NUM_THREADS;j++)
	{
		//Requests j number of threads 
		omp_set_num_threads(j);
		initialize_vectors();

		double begin = omp_get_wtime();
		
		//Runs the DAXPY operation in parallel
		#pragma omp parallel
		{
			//Run by one of the threads to check the actual number of threads  
			#pragma omp single
			num_threads=omp_get_num_threads();
			int i;
			// Worksharing of the for loop construct by the team of threads
			#pragma omp for
			for (i = 0; i < (int)vector_size; i++)
			{
				vectorX[i] = a*vectorX[i] + vectorY[i];
			}
		}
		double end = omp_get_wtime();

		//Time required to perform the DAXPY operation
		double parallel_time = (double)(end - begin);
		
		printf("%lf\n", parallel_time);
		
		//Serial time to calculate speedup
		if(num_threads==1)
			serial_time=parallel_time;

		//Metric to evaluate the performance for every number of threads
		double speedup = serial_time/parallel_time;

		//Maxspeedup calculation
		if(speedup>max_speedup)
		{
			max_speedup=speedup;
			max_speedup_thread_num=num_threads;
		}
		printf("Speedup for %d threads is %lf\n",num_threads,speedup);
	}
	printf("Maximum speedup %lf was obtained for %d threads\n",max_speedup,max_speedup_thread_num);
}