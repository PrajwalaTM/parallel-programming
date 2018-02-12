#include<stdio.h>
#include<cuda_runtime.h>
#include<math.h>

const unsigned int BIN_CAP = 127;
const unsigned int BIN_COUNT = 4096;
const unsigned int ARRAY_SIZE = 1048576;

__global__ void clean_bins(unsigned int *d_bins)
{
    unsigned int threadId = threadIdx.x + blockDim.x*blockIdx.x;
    unsigned int item = d_bins[threadId];
    if(item>BIN_CAP)
        d_bins[threadId] = BIN_CAP;
}

__global__ void local_mem_histo(unsigned int num,unsigned int* d_data,unsigned int* d_bins)
{
    unsigned int threadId = threadIdx.x + blockDim.x*blockIdx.x;
    unsigned int local_bins[BIN_COUNT];
    memset(local_bins,0,sizeof(local_bins));
    unsigned int i,item;
    for (i=0;i<num;i++)
    {
        item = d_data[threadId*num+i];
        local_bins[item]++;
    }
    for(i=0;i<BIN_COUNT;i++)
        atomicAdd(&(d_bins[i]),local_bins[i]);
}

void histo(unsigned int* d_data,unsigned int* d_bins)
{
    unsigned int threads = 1024;
    unsigned int blocks = 1;
    unsigned int num = ARRAY_SIZE/threads;
    local_mem_histo<<<blocks,threads>>>(num,d_data,d_bins);

    threads=1024;
    blocks = BIN_COUNT/threads;
    clean_bins<<<blocks,threads>>>(d_bins);
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
   
    const unsigned int ARRAY_BYTES = ARRAY_SIZE * sizeof(unsigned int);
    const unsigned int BIN_BYTES = BIN_COUNT * sizeof(unsigned int);

    unsigned int h_data[ARRAY_SIZE];
    unsigned int i;
    for(i=0;i<ARRAY_SIZE;i++)
        h_data[i] = rand()%BIN_COUNT;
    
    unsigned int h_bins[BIN_COUNT];

    unsigned int *d_data,*d_bins;
    cudaMalloc((void**)&d_data,ARRAY_BYTES);
    cudaMalloc((void**)&d_bins,BIN_BYTES);

    cudaMemcpy(d_data,h_data,ARRAY_BYTES,cudaMemcpyHostToDevice);
    cudaMemset((void**)&d_bins, 0, BIN_BYTES);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

     //Launch the reduce kernel
     cudaEventRecord(start,0);
     histo(d_data,d_bins);
     cudaEventRecord(stop,0);
     
     cudaEventSynchronize(stop);
     float elapsedTime;
     cudaEventElapsedTime(&elapsedTime,start,stop);
 
     cudaMemcpy(h_bins,d_bins,BIN_BYTES,cudaMemcpyDeviceToHost);
     printf("Value of the bins are\n");
     for(i=0;i<BIN_COUNT;i++)
         printf("%u ",h_bins[i]);
     printf("\nElapsed Time is %f\n",elapsedTime);
 
     cudaFree(d_data);
     cudaFree(d_bins);
     return 0;
 }