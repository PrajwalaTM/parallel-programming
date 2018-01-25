//Q5. Reduction operation - Version 1

#include<stdio.h>
#include<mpi.h>

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //Value sent by all processes is hardcoded to 10
    int value=10,r_value;
    MPI_Status status;

    /* Implementation of peer-to-peer process communication until the master process receives the reduced value
    Process (i) waits for value from process (i+n/2) in a blocking fashion before sending
    partially reduced value to process (i-n/2) */
    
    while(size>1 && rank<size)
    {
       /*In case of odd number of processes, the last process in the set of processes sends the missing element
       to the master process in a blocking fashion*/
       if(size%2 && rank==size-1)
        {
            MPI_Send(&value,1,MPI_INT,0,0,MPI_COMM_WORLD);  
        }
        //Process (i+n/2) sends value to process (i) 
        else if(rank>=(size/2))
            MPI_Send(&value,1,MPI_INT,rank-(size/2),0,MPI_COMM_WORLD);
        else 
        {  
            /*In case of odd number of processes, the master process receives the missing element 
             from the last process in the set of processes in a blocking fashion*/
            if(!rank && size%2)
            {
                MPI_Recv(&r_value,sizeof(int),MPI_INT,size-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
                value=value+r_value;
            }
            //Process (i) receives value from process (i+n/2) 
            MPI_Recv(&r_value,sizeof(int),MPI_INT,rank+(size/2),MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            value = value+r_value;    
        }
        //Bisecting the set of processes
        size/=2;
    }
    //Master process prints the reduced sum
    if(rank==0)
        printf("Sum = %d\n",value);
    MPI_Finalize();
    return 0;
}