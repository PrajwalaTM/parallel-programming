//Q8. Pack and Unpack

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

    char c;
    int i[2];
    float f[4];

    char buffer[110];
    int position=0;
    MPI_Status status;

    if(rank==0)
    {
        c='X';
        i[0]=0;
        i[1]=1;
        f[0]=0.1;
        f[1]=0.2;
        f[2]=0.3;
        f[3]=0.4;

        //All the individual elements of different data types are packed
        MPI_Pack(&c, 1, MPI_CHAR, buffer, 110, &position, MPI_COMM_WORLD);
        MPI_Pack(i, 2, MPI_INT, buffer, 110, &position, MPI_COMM_WORLD);
        MPI_Pack(f, 4, MPI_FLOAT, buffer, 110, &position, MPI_COMM_WORLD);
        int j;

        //Root process sends the packed data to all other processes
        for(j=1;j<size;j++)
            MPI_Send(buffer, position, MPI_PACKED, j, 0, MPI_COMM_WORLD);
    }
    else
    {       
        //Every other process receives the packed data and unpacks it and prints on console  
        MPI_Recv(buffer, 110, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Unpack(buffer, 110, &position, &c, 1, MPI_CHAR, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 110, &position, i, 2, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 110, &position, f, 4, MPI_FLOAT, MPI_COMM_WORLD);;
        printf("Rank is %d\n",rank);
        printf("Character is %c\n",c);
        printf("Integer array is %d\t%d\n",i[0],i[1]);
        printf("Float array is %f\t%f\t%f\t%f\n",f[0],f[1],f[2],f[3]); 
    }
MPI_Finalize();
return 0;
}