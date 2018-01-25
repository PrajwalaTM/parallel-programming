//Q2. DAXPY Loop

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#define vector_size 1<<16

int main(int argc,char *argv[])
{
    int rank,size;
    double start,end;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //Creates a barrier to ensure that all processes start off almost at the same time to measure the parallel
    //time accurately

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    //Split the number of DAXPY operations among the processes
    int elements_per_proc = vector_size/size;
    int i;
    double a = 10;
    int *sub_X = malloc(sizeof(int) * elements_per_proc);
    int *sub_Y = malloc(sizeof(int) * elements_per_proc);
    
    //Each process performs the DAXPY operation for only elements_per_proc iterations
    for(i=0;i<elements_per_proc;i++)
    {
        sub_X[i]=rand()%101;
        sub_Y[i]=rand()%501;
        sub_X[i]=a*sub_X[i]+sub_Y[i];     
    }
    //Creates a barrier to compute the ending parallel time
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    
    if(rank==0)
    {
        printf("Time = %lf\n",end-start);
    }
    MPI_Finalize();
    return 0;
}