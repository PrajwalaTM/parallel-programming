//Q10. Matrix Multiplication on a Cartesian Grid (2D Mesh) using Cannon’s Algorithm
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

//Data about the cartesian grid of processes
struct{
int N; // The number of processors in a row (column)
int size; // Number of processors (Size = N*N)
int row; // This processor’s row number
int col; // This processor’s column number
int MyRank; // This processor’s unique identifier
MPI_Comm comm; // Communicator for all processors in the grid
MPI_Comm row_comm; // All processors in this processor’s row 
MPI_Comm col_comm; // All processors in this processor’s column
}grid_info;

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status status;

    MPI_Comm comm_cart;
    int i,j;

    //Number of dimensions (X,Y)
    int ndims = 2;

    //Array storing the number of processes in each of the dimensions
    int *dims = (int*)malloc(sizeof(int)*ndims);
    dims[0]=sqrt(size);
    dims[1]=sqrt(size);

    //Logical array to mark periodicity of the grid
    int *periods = (int*)malloc(sizeof(int)*ndims);
    periods[0]=1;
    periods[1]=1;

    //Boolean value set to false to not reorder the ranks in the new communicator
    int reorder = 0;

    //Creates a new communicator to which a cartesian grid is attached
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims ,periods, reorder, &comm_cart);

    grid_info.N = sqrt(size);
    grid_info.size = size;
    grid_info.comm = comm_cart;
    grid_info.MyRank = rank;

    int N = grid_info.N;

    //Arrays A and B are multiplied and product is stored in C
    int A[N][N],B[N][N],C[N][N];
    
    //Master process initialises A and B
    if(rank==0)
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                A[i][j]=i*N+j+1;
                B[i][j]=i*N+j+1;
            }
        }
    }

    //A and B values are scattered element wise to each of the processes on the grid
    //Each process receives a single element
    int a,b,c;
    MPI_Scatter(A, 1, MPI_INT, &a, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, 1, MPI_INT, &b, 1, MPI_INT, 0, MPI_COMM_WORLD);

    
    int *row_dims = (int*)malloc(sizeof(int)*grid_info.N);
    row_dims[0]=1;row_dims[1]=0;

    int *col_dims = (int*)malloc(sizeof(int)*grid_info.N);
    col_dims[0]=0;col_dims[1]=1;

    //Create communicators containing processes from a each row and each column
    MPI_Comm row_comm,col_comm;
    MPI_Cart_sub(comm_cart,row_dims,&row_comm);
    MPI_Cart_sub(comm_cart,col_dims,&col_comm);

    grid_info.row_comm=row_comm;
    grid_info.col_comm=col_comm;

    MPI_Comm_rank(grid_info.col_comm, &grid_info.row);
    MPI_Comm_rank(grid_info.row_comm, &grid_info.col);

    int coordinates[2];
    MPI_Cart_coords(grid_info.comm, grid_info.MyRank, ndims, coordinates);
    //printf("Co-ordinates of %d is %d and %d\n",rank,coordinates[0],coordinates[1]);


    //Skewing and rotating operations are performed by send and receive operations among processes
    c=0;
    for(i=0;i<grid_info.N;i++)
    {
        if(!i)
        {
            MPI_Sendrecv_replace(&a, 1, MPI_INT, (grid_info.col - grid_info.row + grid_info.N) % grid_info.N, 0,(grid_info.col + grid_info.row) % grid_info.N, 0, grid_info.row_comm, &status);
            MPI_Sendrecv_replace(&b, 1, MPI_INT, (grid_info.row - grid_info.col + grid_info.N) % grid_info.N , 0, (grid_info.row + grid_info.col) % grid_info.N, 0, grid_info.col_comm, &status);    
        }
        else
        {
            MPI_Sendrecv_replace(&a, 1, MPI_INT, (grid_info.col - 1 + grid_info.N) % grid_info.N, 0,(grid_info.col + 1) % grid_info.N, 0, grid_info.row_comm, &status);
            MPI_Sendrecv_replace(&b, 1, MPI_INT, (grid_info.row - 1 + grid_info.N) % grid_info.N , 0, (grid_info.row + 1) % grid_info.N, 0, grid_info.col_comm, &status);
        }
        c+=a*b;
    }

    //The product is gathered in C
    MPI_Gather(&c,1,MPI_INT,C,1,MPI_INT,0,MPI_COMM_WORLD);
    if(rank==0)
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            printf("%d ",C[i][j]);
            printf("\n");
        }
    }
    MPI_Finalize();
    return 0;
}



