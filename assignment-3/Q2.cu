#include<stdio.h>
#include<cuda_runtime.h>
#include<stdlib.h>


__global__ void reduce_kernel(float *d_out,float *d_in)
{
    //Size of shared memory is set by third parameter of kernel launch
    extern __shared__ float shared_array[];

    int globalThreadId = threadIdx.x + blockDim.x*blockIdx.x;
    int blockThreadId = threadIdx.x;

    shared_array[blockThreadId] = d_in[globalThreadId];
    int s;

    //Sync to ensure full shared_array is loaded
    __syncthreads();

    //Actual reduction operation
    for(s = blockDim.x/2;s>0;s/=2)
    {
        if(blockThreadId<s)
            shared_array[blockThreadId]+=shared_array[blockThreadId+s];
        __syncthreads();
    }
    //Output of reduction is written to first index of global memory
    if(blockThreadId==0)
        d_out[blockIdx.x] = shared_array[0];
}
void reduce(float *d_in,float *d_intermediate,float *d_out,int array_size)
{
    int threads = 256;
    int blocks = array_size/threads;
    reduce_kernel<<<blocks,threads,threads*sizeof(float)>>>(d_intermediate,d_in);

    //Results of all blocks are stored in one block, which has to be reduced
    threads = blocks;
    blocks = 1;
    reduce_kernel<<<blocks,threads,threads*sizeof(float)>>>(d_out,d_intermediate);
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
    
    const int ARRAY_SIZE = 1024;
    const int ARRAY_BYTES = ARRAY_SIZE*sizeof(float);

    float h_in[ARRAY_SIZE];
    int i;
    for(i=0;i<ARRAY_SIZE;i++)
        h_in[i]=i;
    
    float *d_in,*d_out,*d_intermediate;

    cudaMalloc((void**)&d_in,ARRAY_BYTES);
    cudaMalloc((void**)&d_intermediate,ARRAY_BYTES);
    cudaMalloc((void**)&d_out,sizeof(float));

    cudaMemcpy(d_in,h_in,ARRAY_BYTES,cudaMemcpyHostToDevice);
    
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    //Launch the reduce kernel
    cudaEventRecord(start,0);
    reduce(d_in,d_intermediate,d_out,ARRAY_SIZE);
    cudaEventRecord(stop,0);
    
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime,start,stop);

    float h_out;
    cudaMemcpy(&h_out,d_out,sizeof(float),cudaMemcpyDeviceToHost);
    printf("Sum of all array elements is %f\nElapsed time is %f\n",h_out,elapsedTime);
    cudaFree(d_in);
    cudaFree(d_intermediate);
    cudaFree(d_out);
    return 0;
}