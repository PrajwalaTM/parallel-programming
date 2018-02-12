#include<stdio.h>

int main()
{
    int num_devices,i;
    
    cudaGetDeviceCount(&num_devices);
    for(i=0;i<num_devices;i++)
    {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop,i);
        printf("Device Number: %d\n",i);
        printf("Device Name: %s\n",prop.name);
        printf("Compute Capability: Major - %d Minor - %d\n",prop.major,prop.minor);
        printf("Memory Clock Rate(kHz): %d\n",prop.memoryClockRate);
        printf("Memory Bus Width:(bits): %d\n",prop.memoryBusWidth);
        printf("Multiprocessor Count: %d\n",prop.multiProcessorCount);
        printf("Total Global Memory Size(MB): %zu\n",prop.totalGlobalMem/(1024*1024));
        printf("Total Shared Memory Size(KB): %zu\n",prop.sharedMemPerBlock/(1024));
        printf("Total Constant Memory Size(KB): %zu\n",prop.totalConstMem/(1024));
        printf("Number of Registers Per Block: %d\n",prop.regsPerBlock);
        printf("Warp Size: %d\n",prop.warpSize);
        printf("Maximum Number of Threads Per Block: %d\n",prop.maxThreadsPerBlock);
        printf("Maximum Size of a Block: %d %d %d\n",prop.maxThreadsDim[0],prop.maxThreadsDim[1],prop.maxThreadsDim[2]);
        printf("Maximum Size of Grid: %d %d %d\n",prop.maxGridSize[0],prop.maxGridSize[1],prop.maxGridSize[2]);
        printf("Clock Rate(kHz): %f\n",prop.clockRate);
    }
    return 0;
}