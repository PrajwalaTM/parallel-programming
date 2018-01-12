#include<omp.h>
#include<stdio.h>
int main(){
    int num_steps = 100000;
    double step = 1/(double)num_steps;
    double sum=0.0;
    int i;
    double x;
    #pragma omp parallel for private(x) reduction(+:sum)
    for(i=0;i<num_steps;i++)
    {
        x=(i+0.5)*step;
        sum+= 4.0/(1+x*x);        
    }
    double PI = sum*step;
    printf("PI calculated is %f\n",PI);
    return 0;
}