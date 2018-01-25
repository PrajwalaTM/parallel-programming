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
    
    //Requests NUM_THREADS
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

        //Number of iterations run by each of the threads	
        int interval_size = num_steps/actual_num_threads;
    	
    	double x;
    	int low,high;
    	
    	// low and high store the lower and upper limits of the intervals of each of the threads
    	low = thread_ID*interval_size;
    	high = (thread_ID+1)*interval_size;

    	printf("Thread ID is %d, low is %d, high is %d\n",thread_ID,low,high);
    	
    	//Holds the partial sum calculated by each of the threads
    	double partial_sum=0;
    	for(i=low;i<high;i++)
    	{
        	x=(i+0.5)*step;
        	partial_sum+= 4.0/(1+x*x);        
    	}

    	//Updates the shared sum variable atomically
    	#pragma omp atomic
        	sum+=partial_sum;
    }

    //Multiplying sum with step to obtain the area under the curve which is equal to PI
    double PI=sum*step;
    printf("PI calculated is %f\n",PI);
    return 0;
}