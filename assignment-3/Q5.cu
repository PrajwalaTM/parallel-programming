#include<stdio.h>
#include<cuda_runtime.h>
#include<math.h>

#define CHANNELS 3
#define SIZE 256
#define PI 3.14

float h_rgb[SIZE*SIZE*CHANNELS],h_grayscale[SIZE*SIZE];

__global__ void grayscale_kernel(float *d_rgb,float *d_grayscale)
{
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x;
    int by = blockIdx.y;

    int px = bx*blockDim.x+tx;
    int py = by*blockDim.y+ty;

    int ch,i,j;
    float r = d_rgb[py*SIZE*CHANNELS+px*CHANNELS+0];
    float g = d_rgb[py*SIZE*CHANNELS+px*CHANNELS+1];
    float b = d_rgb[py*SIZE*CHANNELS+px*CHANNELS+2];

    d_grayscale[py*SIZE+px] = 0.21*r + 0.71*g + 0.07*b; 
}

void convert_rgb_grayscale(float *d_rgb,float *d_grayscale)
{
    dim3 dimBlock(32,32,1);
    dim3 dimGrid(SIZE/dimBlock.x,SIZE/dimBlock.y);
    grayscale_kernel<<<dimGrid,dimBlock>>>(d_rgb,d_grayscale);
}

void initialise_image()
{
    int i,j,k;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            for(k=0;k<CHANNELS;k++)
            h_rgb[i*SIZE*CHANNELS + j*CHANNELS + k] =rand()%256;
        }
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

    initialise_image();
    float *d_rgb,*d_grayscale;

    const int RGB_BYTES = SIZE*SIZE*CHANNELS*sizeof(float);
    const int GRAYSCALE_BYTES = SIZE*SIZE*sizeof(float);

    cudaMalloc((void**)&d_rgb,RGB_BYTES);
    cudaMalloc((void**)&d_grayscale,GRAYSCALE_BYTES);

    cudaMemcpy(d_rgb,h_rgb,RGB_BYTES,cudaMemcpyHostToDevice);
    
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start,0);
    convert_rgb_grayscale(d_rgb,d_grayscale);
    cudaEventRecord(stop,0);
    
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime,start,stop);

    cudaMemcpy(h_grayscale,d_grayscale,GRAYSCALE_BYTES,cudaMemcpyDeviceToHost);
    printf("Elapsed time is %f\n",elapsedTime);
    
    cudaFree(d_grayscale);
    cudaFree(d_rgb);
    return 0;
}
