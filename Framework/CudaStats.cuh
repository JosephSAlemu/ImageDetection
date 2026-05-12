//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------
#ifndef CUDA_STATS_H
#define CUDA_STATS_H
#include "CudaFramework.h"

void printDevProp(cudaDeviceProp devProp)
{
	Cuda::Trace::out("%s\n", devProp.name);
	Cuda::Trace::out("Major revision number:         %d\n", devProp.major);
	Cuda::Trace::out("Minor revision number:         %d\n", devProp.minor);
	Cuda::Trace::out("Total global memory:           %u bytes\n", devProp.totalGlobalMem);
	Cuda::Trace::out("Number of multiprocessors:     %d\n", devProp.multiProcessorCount);
	Cuda::Trace::out("Total shared memory per block: %u\n", devProp.sharedMemPerBlock);
	Cuda::Trace::out("Total registers per block:     %d\n", devProp.regsPerBlock);
	Cuda::Trace::out("Warp size:                     %d\n", devProp.warpSize);
	Cuda::Trace::out("Maximum memory pitch:          %u\n", devProp.memPitch);
	Cuda::Trace::out("Total constant memory:         %u\n", devProp.totalConstMem);
	return;
}

int getSPcores(cudaDeviceProp devProp)
{
	int cores = 0;
	int mp = devProp.multiProcessorCount;
	switch(devProp.major)
	{
		case 2: // Fermi
			if(devProp.minor == 1) cores = mp * 48;
			else cores = mp * 32;
			break;
		case 3: // Kepler
			cores = mp * 192;
			break;
		case 5: // Maxwell
			cores = mp * 128;
			break;
		case 6: // Pascal
			if((devProp.minor == 1) || (devProp.minor == 2)) cores = mp * 128;
			else if(devProp.minor == 0) cores = mp * 64;
			else Cuda::Trace::out("Unknown device type\n");
			break;
		case 7: // Volta and Turing
			if((devProp.minor == 0) || (devProp.minor == 5)) cores = mp * 64;
			else Cuda::Trace::out("Unknown device type\n");
			break;
		case 8: // Ampere
			if(devProp.minor == 0) cores = mp * 64;
			else if(devProp.minor == 6) cores = mp * 128;
			else if(devProp.minor == 9) cores = mp * 128; // ada lovelace
			else Cuda::Trace::out("Unknown device type\n");
			break;
		case 9: // Hopper
			if(devProp.minor == 0) cores = mp * 128;
			else Cuda::Trace::out("Unknown device type\n");
			break;
		case 10: // Blackwell
			if(devProp.minor == 0) cores = mp * 128;
			else Cuda::Trace::out("Unknown device type\n");
			break;
		case 12: // Blackwell
			if(devProp.minor == 0) cores = mp * 128;
			else Cuda::Trace::out("Unknown device type\n");
			break;
		default:
			Cuda::Trace::out("Unknown device type\n");
			break;
	}
	return cores;
}

#endif

// --- End of File ----
