#include<stdio.h>
#include<cuda_runtime.h>
#include<math.h>

#define SIZE 1024
#define TILE_WIDTH 16

float h_M[SIZE*SIZE],h_N[SIZE*SIZE],h_P[SIZE*SIZE];

__global__ void multiplication_kernel(float *d_M,float *d_N,float *d_P)
{
__shared__ float ds_M[TILE_WIDTH][TILE_WIDTH];
__shared__ float ds_N[TILE_WIDTH][TILE_WIDTH];

int tx = threadIdx.x;
int ty = threadIdx.y;
int bx = blockIdx.x;
int by = blockIdx.y;

int col = TILE_WIDTH * bx + tx;
int row = TILE_WIDTH * by + ty;

float prod_value = 0;
int m,k;
for(m=0;m<SIZE/TILE_WIDTH;m++)
{
    ds_M[ty][tx] = d_M[row*SIZE+(m*TILE_WIDTH+tx)];
    ds_N[ty][tx] = d_N[(m*TILE_WIDTH+ty)*SIZE+col];
    __syncthreads();

    for(k=0;k<TILE_WIDTH;k++)
        prod_value+=ds_M[ty][k]*ds_N[k][tx];
    __syncthreads();
}
d_P[row*SIZE+col] = prod_value;
}

void matrix_multiplication(float *d_M,float *d_N,float *d_P)
{
    dim3 dimBlock(TILE_WIDTH,TILE_WIDTH,1);
    dim3 dimGrid(SIZE/TILE_WIDTH,SIZE/TILE_WIDTH,1);

    multiplication_kernel<<<dimGrid,dimBlock>>>(d_M,d_N,d_P);
}

void display_matrix(float mat[])
{
    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
            printf("%f ",mat[i*SIZE+j]);
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
    
    const int ARRAY_BYTES = SIZE*SIZE*sizeof(float);
    float *d_M,*d_N,*d_P;
    cudaMalloc((void**)&d_M,ARRAY_BYTES);
    cudaMalloc((void**)&d_N,ARRAY_BYTES);
    cudaMalloc((void**)&d_P,ARRAY_BYTES);

    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            h_M[i*SIZE+j] = rand()%101;
            h_N[i*SIZE+j] = rand()%101;
        }
    }
    cudaMemcpy(d_M,h_M,ARRAY_BYTES,cudaMemcpyHostToDevice);
    cudaMemcpy(d_N,h_N,ARRAY_BYTES,cudaMemcpyHostToDevice);
    
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start,0);
    matrix_multiplication(d_M,d_N,d_P);
    cudaEventRecord(stop,0);
    
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime,start,stop);

    cudaMemcpy(h_P,d_P,ARRAY_BYTES,cudaMemcpyDeviceToHost);
    /*
    printf("M is \n");
    display_matrix(h_M);
    printf("N is \n");
    display_matrix(h_N);
    printf("Product of M and N is \n");
    display_matrix(h_P);
    */
    printf("Elapsed time is %f\n",elapsedTime);
    cudaFree(d_M);
    cudaFree(d_N);
    cudaFree(d_P);
    return 0;
}