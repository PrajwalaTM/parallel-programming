//Q4. Matrix Multiply

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 1000
#define NUM_THREADS 16
int A[N][N],B[N][N],C[N][N];

//Initializing the matrices with random values 
void initialize_matrices()
{
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			A[i][j] = (rand()%11);
			B[i][j]	= (rand()%11);
		}
	}
}

int main()
{
	int i,j,k,m;
	double max_speedup=0,serial_time;
	int max_speedup_thread_num=0;
	int num_threads;

	// Loop that evaluates the performance using 1,2,3...NUM_THREADS threads 
	for(m=1;m<=NUM_THREADS;m++)
	{
		omp_set_num_threads(m);
		initialize_matrices();
		double begin = omp_get_wtime();

		//Calculates the matrix product with worksharing for loop construct
		#pragma omp parallel 
		{
			#pragma omp single
				num_threads=omp_get_num_threads();

			//For loop for evaluation of the matrix product values 
			// j,k are made private variables for each thread	
			#pragma omp for private(k,j)
			for (i = 0; i < N ; i++)
			{
				for (j = 0; j < N ; j++)
				{
					C[i][j]=0;
					for (k = 0; k < N; k++)
					{
						C[i][j] = A[i][k]*B[k][j] + C[i][j];
					}
				}	
			}
		}

		double end = omp_get_wtime();

		//Time required to perform the matrix multiplication 
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