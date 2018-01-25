//Q4. Calculation of π - MPI Bcast and MPI Reduce
#include<stdio.h>
#include<mpi.h>

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (size < 2) 
    {
    fprintf(stderr, "Size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int num_steps; 
    double pi,start,end; 
    if(rank==0)
    {
        start = MPI_Wtime();
        num_steps = 100000;
    }
    //Process with rank 0 broadcasts the num_steps to all other processes
    MPI_Bcast(&num_steps,sizeof(int),MPI_INT,0,MPI_COMM_WORLD);

    double partial_pi=0,partial_sum=0,x;
    double step = 1/(double)num_steps;
    int i;
    //Each of the processes including master process compute the partial sum value
    for(i=rank-1;i<num_steps;i+=size)
    {
        x = (i+0.5)*step;
        partial_sum+=4.0/(1+x*x);
    }
    //Computing the partial pi value
    partial_pi = partial_sum*step;
    printf("Partial pi computed by process %d is %lf\n",rank,partial_pi);

    //Partial PI values sent by all processes are reduced by the master process 
    MPI_Reduce(&partial_pi,&pi,1,MPI_DOUBLE,MPI_SUM,0,
             MPI_COMM_WORLD);

    //The final reduced PI value is printed by master process
    if(rank==0)
    {
    printf("PI calculated is %lf\n",pi);
    end = MPI_Wtime();
    printf("Time = %lf\n",end-start);
    }
    /* Note: There is no requirement of a barrier in this case for parallel time computation as 
     MPI_Reduce() call takes care that all processes are in the same state before executing
     the rest of the code */
     
    MPI_Finalize(); 
    return 0;
}