//Q7.  Calculation of PI- Monte Carlo Simulation

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM_TRIALS 100000
#define MULTIPLIER 1366
#define ADDEND 150889
#define PMOD 714025

//Holds the last random variable to be used to obtain the next pseudo random number
long rand_last=0;

//To make the pseudo random number generator thread safe
#pragma omp threadprivate(rand_last)

//Code for the pseudo random number generator using the LEAP FROG method
double get_random_number()
{
	//rand_next is the next pseudo random number 
	long rand_next = (MULTIPLIER*rand_last+ADDEND)%PMOD;
	rand_last=rand_next;

	return ((double)rand_next/(double)PMOD);
}

int main()
{
	//Holds the random seeds for each of the threads according to ID in order to obtain the
	//same sequence of values regardless of the number of threads
	double *seeds_array;

	//Variable to hold the number of points inside the dart circle
	int num_circle_points=0,num_threads,i;	

	//Calculates the seed values and the number of points inside the circle in parallel
	#pragma omp parallel private(i)
	{
		num_threads = omp_get_num_threads();
		seeds_array=(double *)malloc(num_threads*sizeof(double));

		//Initial seed for thread ID 0
		int iseed = PMOD/MULTIPLIER,i,mult;
		seeds_array[0]=iseed;
		mult=MULTIPLIER;

		//Calculates the seed value for each of the threads using the Linear Congruential Generator(LCG) method
		for(i=1;i<num_threads;i++)
		{
			iseed=(unsigned long long)((MULTIPLIER*iseed)%PMOD);
			seeds_array[i]=iseed;
			mult = (mult*MULTIPLIER)%PMOD;
		}
		int thread_id=omp_get_thread_num();
		rand_last=seeds_array[thread_id];
		
		//Sets the radius of the circle to 1
		double radius=1.0;

		//Reduction construct to obtain the number of points in the circle
		#pragma omp parallel for reduction(+:num_circle_points)
		for(i=0;i<NUM_TRIALS;i++)
		{
			//Obtain the x,y co-ordinates of the point using the pseudo random number generator
			double x = get_random_number();
			double y = get_random_number();

			//Condition to check if point (x,y) lies on or inside the circle 
			if(x*x+y*y<=radius*radius)
				num_circle_points++;
		}
	}
		//The value of PI is calculated using the formula - 4*PI*(r^2)/(4*(r^2))
		double PI = 4.0*((double)num_circle_points/(double)NUM_TRIALS);
		printf("Number of trials is %d and PI calculated is %lf\n",NUM_TRIALS,PI);
		return 0;
}