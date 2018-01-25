//Q5. Reduction operation - Version 2
#include<stdio.h>
#include<mpi.h>

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int value=10,r_value,ierr;
    MPI_Status   status;
    
    //Used for MPI_Wait() to ensure that all non-blocking operations are completed
    MPI_Request	send_request,recv_request;

    /*Implementation of peer-to-peer process communication until the master process receives the reduced value
    Each send and receive is a non-blocking operation and a wait operation is performed to 
    ensure that process (i) sends to process (i-n/2) only after it receives from process (i+n/2) */
    while(size>1 && rank<size)
    {
        /*In case of odd number of processes, the last process in the set of processes sends the missing element
       to the master process in a non-blocking fashion*/
       if(size%2 && rank==size-1)
        {
            ierr=MPI_Isend(&value,1,MPI_INT,
	           0,0,MPI_COMM_WORLD,&send_request); 
            ierr=MPI_Wait(&send_request,&status);  
        }
        //Process (i+n/2) sends value to process (i)  
        else if(rank>=(size/2))
        {   ierr=MPI_Isend(&value,1,MPI_INT,
	           rank-(size/2),0,MPI_COMM_WORLD,&send_request);
            ierr=MPI_Wait(&send_request,&status);
        } 
        /*In case of odd number of processes, the master process receives the missing element 
        from the last process in the set of processes in a non-blocking fashion*/
        else 
        {   
            if(!rank && size%2)
            {
                ierr=MPI_Irecv(&r_value,1,MPI_INT,
	            size-1,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request);  
                ierr=MPI_Wait(&recv_request,&status);
                value=value+r_value;
            }
            //Process (i) receives value from process (i+n/2) 
            ierr = MPI_Irecv(&r_value,1,MPI_INT,rank+(size/2),MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request);
            ierr=MPI_Wait(&recv_request,&status);
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