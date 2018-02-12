#include<stdio.h>
#include<cuda_runtime.h>
#include<math.h>

#define CHANNELS 3
#define BLUR_SIZE 3
#define SIZE 12
#define PI 3.14

float h_image[SIZE*SIZE*CHANNELS],h_filter[BLUR_SIZE*BLUR_SIZE],h_blurredimage[SIZE*SIZE*CHANNELS];

__global__ void gaussian_blur(float *d_image,float *d_blurredimage,float* d_filter)
{
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x;
    int by = blockIdx.y;

    int px = bx*blockDim.x+tx;
    int py = by*blockDim.y+ty;

    float c;
    int fx,fy,ch,imgx,imgy;
    for(ch=0;ch<CHANNELS;ch++)
    {
        c=0;
        for(fx=0;fx<BLUR_SIZE;fx++)
        {
            for(fy=0;fy<BLUR_SIZE;fy++)
            {
                imgx = px + fx - BLUR_SIZE/2;
                imgy = py + fy - BLUR_SIZE/2;
                imgx = min(max(imgx,0),SIZE-1);
                imgy = min(max(imgy,0),SIZE-1);
                c+=d_filter[fy*BLUR_SIZE+fx]*d_image[imgy*SIZE*CHANNELS+imgx*CHANNELS+ch];
            }
        }
        d_blurredimage[py*SIZE*CHANNELS+px*CHANNELS+ch] = c;
    }
}
void blur(float *d_image,float *d_blurredimage,float *d_filter)
{
    dim3 dimBlock(4,4,1);
    dim3 dimGrid(SIZE/dimBlock.x,SIZE/dimBlock.y);
    gaussian_blur<<<dimGrid,dimBlock>>>(d_image,d_blurredimage,d_filter);
}

void initialise_image()
{
    int i,j,k;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            for(k=0;k<CHANNELS;k++)
            h_image[i*SIZE*CHANNELS + j*CHANNELS + k] =rand()%256;
        }
    }
}
void initialise_filter()
{
    int i,j;
    double sum = 0.0,r,s=1/(2*PI);

    for(i=-BLUR_SIZE/2;i<=BLUR_SIZE/2;i++)
    {
        for(j=-BLUR_SIZE/2;j<=BLUR_SIZE/2;j++)
        {
        r = exp(-(i*i+j*j))/2;
        h_filter[(i+2)*BLUR_SIZE+(j+2)] = r*s;
        sum +=h_filter[(i+2)*BLUR_SIZE+(j+2)];
        }  
    }
    for(i=0;i<BLUR_SIZE;i++)
    {
        for(j=0;j<BLUR_SIZE;j++)
            h_filter[i*BLUR_SIZE+j]/=sum;
    }
}
int main(int argc, char *argv[])
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    if(!deviceCount){
        fprintf(stderr,"No devices supporting cuda\n");
        exit(EXIT_FAILURE);
    }
    int deviceId = 0;
    cudaSetDevice(deviceId);

    printf("Initialising\n");
    initialise_image();
    initialise_filter();

    float *d_image,*d_blurredimage,*d_filter;

    const int ARRAY_BYTES = SIZE*SIZE*CHANNELS*sizeof(float);
    const int FILTER_BYTES = BLUR_SIZE*BLUR_SIZE*sizeof(float);

    cudaMalloc((void**)&d_image,ARRAY_BYTES);
    cudaMalloc((void**)&d_blurredimage,ARRAY_BYTES);
    cudaMalloc((void**)&d_filter,FILTER_BYTES);

    cudaMemcpy(d_image,h_image,ARRAY_BYTES,cudaMemcpyHostToDevice);
    cudaMemcpy(d_filter,h_filter,FILTER_BYTES,cudaMemcpyHostToDevice);
    
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start,0);
    blur(d_image,d_blurredimage,d_filter);
    cudaEventRecord(stop,0);
    
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime,start,stop);

    cudaMemcpy(h_blurredimage,d_blurredimage,ARRAY_BYTES,cudaMemcpyDeviceToHost);
    printf("Elapsed time is %f\n",elapsedTime);
    
    cudaFree(d_image);
    cudaFree(d_blurredimage);
    return 0;
}