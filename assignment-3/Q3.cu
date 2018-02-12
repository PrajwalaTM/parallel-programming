#include<stdio.h>
#include<cuda_runtime.h>

#define SIZE 1000
int h_M[SIZE*SIZE],h_N[SIZE*SIZE],h_S[SIZE*SIZE];

__global__ void addition_kernel(int *d_M,int *d_N,int *d_S)
{
    int tIdx = blockDim.x*blockIdx.x + threadIdx.x;
    int tIdy = blockDim.y*blockIdx.y + threadIdx.y;
    int s= d_M[tIdy*SIZE+tIdx] + d_N[tIdy*SIZE+tIdx];
    d_S[tIdy*SIZE+tIdx] = s;
}
void matrix_addition(int *d_M,int *d_N,int *d_S)
{
    dim3 dimBlock(32,32,1);
    dim3 dimGrid(SIZE/dimBlock.x,SIZE/dimBlock.y);
    addition_kernel<<<dimGrid,dimBlock>>>(d_M,d_N,d_S);
}
void display_matrix(int mat[])
{
    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
            printf("%d ",mat[i*SIZE+j]);
        printf("\n");
    }
}
int main()
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    if(!deviceCount){
        fprintf(stderr,"No devices supporting cuda\n");
        exit(EXIT_FAILURE);
    }
    int deviceId = 0;
    cudaSetDevice(deviceId);
    
    const int ARRAY_BYTES = SIZE*SIZE*sizeof(int);
    int *d_M,*d_N,*d_S;
    cudaMalloc((void**)&d_M,ARRAY_BYTES);
    cudaMalloc((void**)&d_N,ARRAY_BYTES);
    cudaMalloc((void**)&d_S,ARRAY_BYTES);

    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            h_M[i*SIZE+j] = i*SIZE+j;
            h_N[i*SIZE+j] = j*SIZE+i;
        }
    }
    cudaMemcpy(d_M,h_M,ARRAY_BYTES,cudaMemcpyHostToDevice);
    cudaMemcpy(d_N,h_N,ARRAY_BYTES,cudaMemcpyHostToDevice);
    
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start,0);
    matrix_addition(d_M,d_N,d_S);
    cudaEventRecord(stop,0);
    
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime,start,stop);

    cudaMemcpy(h_S,d_S,ARRAY_BYTES,cudaMemcpyDeviceToHost);
    /*printf("M is \n");
    display_matrix(h_M);
    printf("N is \n");
    display_matrix(h_N);
    printf("Addition of M and N is \n");
    display_matrix(h_S);*/

    printf("Elapsed time is %f\n",elapsedTime);
    cudaFree(d_M);
    cudaFree(d_N);
    cudaFree(d_S);
    return 0;
}