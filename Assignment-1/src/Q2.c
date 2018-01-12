//Q2. Hello World Program - Version 2

#include <stdio.h>
#include <omp.h>

//Function that prints "Hello World" with thread ID
void printHello(int thread_id)
{
	printf("Hello World %d\n", thread_id);
}

int main()
{
	//Calls the printHello() function for each thread in the team
	#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		printHello(ID);
	}
}