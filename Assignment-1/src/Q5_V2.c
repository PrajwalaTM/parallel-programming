//Q5. Calculation of PI

#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 4

int main()
{
    //Dividing the interval 0-1 into 100000 sub-intervals
    int num_steps = 100000;

    //Length of each interval
    double step = 1/(double)num_steps;
    
    //Shared variable that holds the sum of the values of the function 4/1+x^2
    double sum=0;
    omp_set_num_threads(NUM_THREADS);
    
    //Verify that the number of threads requested is provided
    int actual_num_threads;

    #pragma omp parallel
    {
    	int i;
    	int thread_ID=omp_get_thread_num();

        //Obtain the actual number of threads set
    	#pragma omp single
        	actual_num_threads=omp_get_num_threads();
    	double x;
    	
        //Holds the partial sum calculated by each of the threads
        double partial_sum=0;

        //Threads are allocated iterations in a round robin fashion
    	for(i=thread_ID;i<num_steps;i=i+actual_num_threads)
    	{
        	x=(i+0.5)*step;
        	partial_sum+= 4.0/(1+x*x);        
    	}
        //Updates the shared sum variable using critical section
    	#pragma omp critical
        {
        	sum+=partial_sum;
        }
    }

    //Multiplying sum with step to obtain the area under the curve which is equal to PI
    double PI=sum*step;
    printf("PI calculated is %f\n",PI);
    return 0;
}