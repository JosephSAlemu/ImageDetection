//----------------------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------

#ifndef _CudaFramework_h
#define _CudaFramework_h

// The main CUDA runtime header must be included after defining __CUDACC__
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

// Cuda error handling
#include <stdio.h>
#include <stdlib.h>
#include "Framework.h"
namespace Cuda
{
	namespace Trace
	{
		static void out(const char *const fmt, ...) noexcept
		{
			va_list args;

			static std::mutex mut;
			std::lock_guard<std::mutex> _guard(mut);

#pragma warning( push )
#pragma warning( disable : 26492 )
#pragma warning( disable : 26481 )
			va_start(args, fmt);
#pragma warning( pop )
			static char buffer[512];
			vsprintf_s(&buffer[0], 512, fmt, args);
			OutputDebugStringA(&buffer[0]);

			// va_end(args); - original.. below to new code
			args = static_cast<va_list> (nullptr);
		}
	}

}

#pragma warning( push )
#pragma warning( disable : 4505 )
static void CudaAssert(cudaError_t err, const char *file, int line) noexcept
{
	if(err != cudaSuccess)
	{
		Trace::out("%s in \n%s(%d) : <double-click>\n\n", cudaGetErrorString(err), file, line);
		exit(1);
	}
}
#pragma warning( pop )

// If the cuda function fails, prints the error and exits the process
#define CudaTry( err ) (CudaAssert( err, __FILE__, __LINE__ ))


#endif

// --- End of File ----
