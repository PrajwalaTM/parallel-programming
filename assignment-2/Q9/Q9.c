//Q9. Derived Datatype - Indexed

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status status;

    MPI_Datatype old_type,new_type;
    old_type = MPI_INT;

    //Number of blocks
    int N = 4;
    
    //Array that stores the number of elements of each block
    int *array_of_blocklengths = (int*)malloc(sizeof(int)*N);

    //Array that stores the displacement of each of the blocks
    int *array_of_displacements = (int*)malloc(sizeof(int)*N);
    int i,j,k;

    //A 1D array that stores all the elements of the matrix
    int *a = (int*)malloc(N*N*sizeof(int));

    //Initialising the arrays to send only the upper triangular matrix elements
    for(i=0;i<N;i++)
    {
        array_of_displacements[i]= N*i+i;
        array_of_blocklengths[i] = N-i;
    }

    //Creation of the indexed data type
    MPI_Type_indexed(N,array_of_blocklengths, array_of_displacements, MPI_INT, &new_type);
    
    //Commits the new data type
    MPI_Type_commit(&new_type);
    
    if(rank==0)
    {
        for(i=0;i<N*N;i++)
            a[i]=i;

        //Master process applies the indexed data type on the matrix to send upper triangular matrix 
        // to process 1      
        MPI_Send(a,1,new_type,1,0,MPI_COMM_WORLD);
    }
    else if(rank==1)
    {   
        //Process 1 receives the upper triangular matrix and prints it
        MPI_Recv(a, 1, new_type, 0, 0, MPI_COMM_WORLD, &status);
        for(i=k=0;i<N;i++)
        {
            for(j=0;j<N;j++)    
                printf("%d ",a[k++]);
            printf("\n");
        }    
    }
    MPI_Finalize();
    return 0;
}  