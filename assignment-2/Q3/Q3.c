//Q3. Hello World Program - Version 2
#include<stdio.h>
#include<mpi.h>

int main(int argc,char *argv[])
{
    int rank,size,namelen;
    char name[100];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(name,&namelen);

    if (size < 2) 
    {
    fprintf(stderr, "Size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char message[11]="Hello World";
    
    //All processes other than the process with rank 0 send "Hello world" to master process
    if(rank!=0)
    {
    MPI_Send(message,11,MPI_CHAR,0,0,MPI_COMM_WORLD);   
    }
    else
    {
        MPI_Status status;
        int i,count;    
        //Master process receives the message from all other processes
        for(i=1;i<size;i++)
        {
        //Probing to get the size of message to be received before receiving the message
        MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &count);
        MPI_Recv(message,count,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);                    
        printf("%s received %d bytes from %d\n",message,count,status.MPI_SOURCE);
        }
    }
    MPI_Finalize();
    return 0;
}