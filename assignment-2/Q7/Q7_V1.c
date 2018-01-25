//Q7. MPI Derived Datatypes Version-1
//Implements the collective communication version

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

//Derived data type
struct dd{
    char c;
    int i[2];
    float f[4];
}s;

int main(int argc,char *argv[])
{
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    MPI_Datatype new_type;

    //Number of blocks in the derived data type
    int count = 3;

    //Number of elements in each block
    int array_of_blocklengths[count]; 
    
    //Array of data types of each block
    MPI_Datatype array_of_types[count];
    MPI_Aint array_of_displacements[count],block1_address,block2_address,block3_address;

    //Retrieves the address of each of the blocks
    MPI_Get_address(&s.c, &block1_address);
    MPI_Get_address(s.i, &block2_address);
    MPI_Get_address(s.f, &block3_address);


    array_of_blocklengths[0] = 1;
    array_of_blocklengths[1] = 2;
    array_of_blocklengths[2] = 4;
    array_of_types[0] = MPI_CHAR;
    array_of_types[1]= MPI_INT;
    array_of_types[2] = MPI_FLOAT;

    //Initialising the displacements of each of the blocks relative to the starting address of the
    //structure
    array_of_displacements[0] = 0;
    array_of_displacements[1] = block2_address-block1_address;
    array_of_displacements[2] = block3_address-block1_address;

    //Creates the derived struct data type
    MPI_Type_create_struct(count,array_of_blocklengths,array_of_displacements,array_of_types,&new_type);
    
    //Commits the new type
    MPI_Type_commit(&new_type);

    //Master process initialises the structure
    if(rank==0)
    {
        s.c='X';
        s.i[0] = 1;
        s.i[1] = 2;
        s.f[0]= 0.0;
        s.f[1] = 0.1;
        s.f[2] = 0.3;
        s.f[3] = 0.4;
    }

    //Master process broadcasts the structure to all other processes
    MPI_Bcast(&s,sizeof(s),new_type,0,MPI_COMM_WORLD);
    if(rank!=0)
    {
        printf("Rank is %d\n",rank);
        printf("Character is %c\n",s.c);
        printf("Integer array is %d\t%d\n",s.i[0],s.i[1]);
        printf("Float array is %f\t%f\t%f\t%f\n",s.f[0],s.f[1],s.f[2],s.f[3]);
    }
    MPI_Finalize();
    return 0;
}

