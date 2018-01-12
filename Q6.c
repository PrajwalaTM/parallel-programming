//Q6.  Calculation of PI- Worksharing and Reduction

#include <omp.h>
#include <stdio.h>

int main()
{   
    //Dividing the interval 0-1 into 100000 sub-intervals
    int num_steps = 100000;

    //Length of each interval
    double step = 1/(double)num_steps;
    
    //Shared variable that holds the sum of the values of the function 4/1+x^2
    double sum=0.0;
    int i;
    double x;

    //Worksharing and reduction constructs that split the iterations among the threads and 
    //update the shared sum variable atomically
    #pragma omp parallel for private(x) reduction(+:sum)
    for(i=0;i<num_steps;i++)
    {
        x=(i+0.5)*step;
        sum+= 4.0/(1+x*x);        
    }

    //Multiplying sum with step to obtain the area under the curve which is equal to PI
    double PI = sum*step;
    printf("PI calculated is %f\n",PI);
    return 0;
}