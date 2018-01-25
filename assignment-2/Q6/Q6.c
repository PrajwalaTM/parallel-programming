//Q6. Collective Communication - Scatter - Gather
#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

//Function called by the master process to initialise the array with random values
void initialise_array(float *arr,int n)
{
    int i;
    for(i=0;i<n;i++)
    arr[i]=rand()%101;
}

//Function to print the array
void print_array(float *arr,int n)
{
    int i;
    for(i=0;i<n;i++)
    printf("%lf ",arr[i]);
    printf("\n");
}

int main(int argc,char *argv[])
{
    int rank,size,i,num=100;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //sendcounts[i] is the number of elements to be sent to process i
    int *sendcounts = (int*)malloc(sizeof(int)*size);

    // cumsendcounts[i] is the number of elements to be sent to processes 1,2,...i
    int *cumsendcounts = (int*)malloc(sizeof(int)*size); 
    
    // displs[i] is the displacement relative to arr from where data should be sent to process i
    int *displs = (int*)malloc(sizeof(int)*size);

    //Data that is scattered non-uniformly
    float *arr = (float*)malloc(sizeof(float)*num);
    
    //Gathered data is stored here
    float *recv_buff = (float*)malloc(sizeof(float)*num);

    //This is to scatter varying amounts of data to odd and even rank processes
    //cumsendcounts[] is used for assigning values to displs[]
    int rem=num; 
    for(i=0;i<size;i++)
    {
        sendcounts[i]=num/size;
        if(i%2)
        sendcounts[i]++;
        else
        sendcounts[i]--;
        rem = rem-sendcounts[i];
        if(i)
        {
            cumsendcounts[i]=cumsendcounts[i-1]+sendcounts[i];
            displs[i]=cumsendcounts[i-1];  
        } 
        else
        {
            cumsendcounts[i]=sendcounts[i];
            displs[i]=0;
        }
            
    }
    //Any remaining elements are assigned to last process
    if(rem)
        sendcounts[size-1]+=rem;

    //Master process initialises the array
    if(rank==0)
    {    
        initialise_array(arr,num);
        printf("Initial array:\n");
        print_array(arr,num);     
    }

    //Array is scattered to all other processes by master process non-uniformly
    MPI_Scatterv(arr,sendcounts,displs,MPI_FLOAT,recv_buff,sendcounts[rank],MPI_FLOAT,0,MPI_COMM_WORLD);
    
    //Square root is performed on the received data by all processes
    for(i=0;i<sendcounts[rank];i++)
    {
        recv_buff[i]=sqrt(recv_buff[i]);
    }

    //Master process gathers the square rooted array values from all other processes
    MPI_Gatherv(recv_buff,sendcounts[rank],MPI_FLOAT,arr,sendcounts,displs,MPI_FLOAT,0,MPI_COMM_WORLD);
    
    //Final array is printed by master process
    if(rank==0)
    {
        printf("After scatter and gather:\n");
        print_array(arr,num);
    }
    MPI_Finalize();
    return 0;
}