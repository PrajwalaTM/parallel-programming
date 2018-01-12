#include<omp.h>
#include<stdio.h>
#define NUM_THREADS 4
int main(){
    int num_steps = 100000;
    double step = 1/(double)num_steps;
    double sum[NUM_THREADS];
    omp_set_num_threads(NUM_THREADS);
    //Verify that the number of threads requested is provided
    int actual_num_threads;
    #pragma omp parallel
    {
    int num_threads = omp_get_num_threads(),i;
    int interval_size = num_steps/num_threads;
    printf("Number of threads is %d\n",num_threads);
    int thread_ID=omp_get_thread_num();
    if(thread_ID==0) 
        actual_num_threads=num_threads;
    double x;
    int low,high;
    low = thread_ID*interval_size;
    high = (thread_ID+1)*interval_size;
    printf("ID is %d, low is %d, high is %d\n",thread_ID,low,high);
    sum[thread_ID]=0;
    for(i=low;i<high;i++)
    {
        x=(i+0.5)*step;
        sum[thread_ID]+= 4.0/(1+x*x);        
    }
    }
    double PI=0;
    int i;
    for(i=0;i<actual_num_threads;i++)
    PI=PI+sum[i];
    PI=PI*step;
    printf("PI calculated is %f\n",PI);
    return 0;
}