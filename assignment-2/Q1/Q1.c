//Q1. Hello World Program - Version 1
#include<stdio.h>
#include<mpi.h>

int main(int argc,char *argv[])
{
    int rank,size,namelen;
    char name[100];
    MPI_Init(&argc,&argv);
    //Initialises the number of processes and the process rank in the communicator
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(name,&namelen);
    //Each process prints out its name and rank
    printf ("Hello World. Rank %d out of %d running on %s!\n", rank, size, name);
    MPI_Finalize();
    return 0;
}