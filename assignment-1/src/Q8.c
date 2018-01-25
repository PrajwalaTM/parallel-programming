#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//Producer function to fill random values in the array that will be consumed by the consumer
void fill_rand(int n, double *array)
{
	int i;
	for(i=0;i<n;i++)
	{
		int r = rand()%1001;
		array[i] = r;
		printf("%lf\n",array[i]);
	}
}

//Consumer function that consumes the array values produced by the producer and calculates their sum
double sum_array(int n, double *array)
{
	int i;
	double sum=0;
	for (i = 0; i < n; i++)
	{
		sum = sum + array[i];	
	}
	return sum;
}

int main()
{
	double *array,sum,parallel_time;
	int flag = 0;
	int n = 25;
	array = (double *)malloc(n*sizeof(double));
	double begin = omp_get_wtime();

	//Sections to create Producer and Consumer thread and perform pairwise thread synchronization
	#pragma omp parallel sections
	{
		//Producer thread section
		#pragma omp section
		{
			fill_rand(n,array);

			//Flushes all the variables to the DRAM
			#pragma omp flush

			//Atomic write to ensure that flag is 1
			#pragma atomic write
				flag = 1;

			//Flushes flag to DRAM to be consumed by the consumer thread	
			#pragma omp flush(flag)
		}

		//Consumer thread section
		#pragma omp section
		{
			int temp_flag;
			//Consumer spins infinitely until flag is set to 1 by the producer
			while(1)
			{
				//Repeatedly flushes the flag value to read it
				#pragma omp flush(flag)

				//Atomic read to obtain the complete value of flag
				#pragma omp atomic read
					temp_flag = flag;

				//Breaks out of the infinite loop when the consumer has set flag to 1 and it has been flushed
				if(temp_flag==1)
					break;
			}
			//Flushes all variables prior to sum calculation
			#pragma omp flush
			sum = sum_array(n,array);
		}
	}
	printf("Sum = %lf\n",sum);

}