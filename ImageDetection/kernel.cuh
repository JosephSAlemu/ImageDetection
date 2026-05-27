#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include "CudaFramework.h"
#include "ImageGray.h"
#include "ImageRGB.h"
#include "Point.h"
#include "Host.h"

// Macros for the pipeline or the individual steps with benchmarks.
#define PIPELINE true

#define THREAD_X 32
#define THREAD_Y 16

#if PIPELINE
void combined_CUDA(ImageRGB& rImageRGBIn, ImageGray& rImageGrayOut, ImageGray& tmp, std::vector<Point>& CenterPoints, int threshOne, int threshTwo, int blurPasses, int dilatePasses, int erodePasses);
#else
void RGB_to_gray_CUDA(ImageGray& rImageGrayOut, ImageRGB& rImageRGBIn);
void threshold_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char threshold);
void invert_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void erode_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void dilate_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void blur_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void dilate_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void erode_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void edge_detection_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void label_coins_CUDA(ImageGray& ImageEdge, ImageRGB& rImageRGBOut, ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints);

#endif
__device__ __constant__ const unsigned int MethodCount = 4;


__device__ __constant__ const unsigned int ErodeKernelOne[] =
{ 
 0, 1, 1, 1, 0,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 0, 1, 1, 1, 0 
};

__device__ __constant__ const unsigned int DilateKernelOne[] =
{
 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0 
};

__device__ __constant__ const unsigned int Guass5x5Kernel[] = 
{ 
 1,  4,  7,  4, 1,
 4, 16, 26, 16, 4,
 7, 26, 41, 26, 7,
 4, 16, 26, 16, 4,
 1,  4,  7,  4, 1 
};

__device__ __constant__ const unsigned int DilateKernelTwo[] =
{
 1, 1, 1,
 1, 1, 1,
 1, 1, 1 
};

__device__ __constant__ const unsigned int ErodeKernelTwo[] =
{ 
 1, 1, 1,
 1, 1, 1,
 1, 1, 1 
};

__device__ __constant__ const float SobelKernel_Gx[] =
{ 
 -1.0f, 0.0f, 1.0f,
 -2.0f, 0.0f, 2.0f,
 -1.0f, 0.0f, 1.0f 
};

__device__ __constant__ const float SobelKernel_Gy[] =
{ 
 -1.0f, -2.0f, -1.0f,
  0.0f,  0.0f,  0.0f,
  1.0f,  2.0f,  1.0f 
};

__device__ __constant__ const unsigned int SobelSize = 3;
__device__ __constant__ const unsigned int SobelApron = 1; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int SobelHalo = 1;  // padding around the center of kernel

__device__ __constant__ const unsigned int ErodeTotalTwo = 255 * 9;
__device__ __constant__ const unsigned int ErodeSizeTwo = 3;
__device__ __constant__ const unsigned int ErodeApronTwo = 1; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int ErodeHaloTwo = 1;  // padding around the center of kernel

__device__ __constant__ const unsigned int DilateTotalTwo = 255 * (9);
__device__ __constant__ const unsigned int DilateSizeTwo = 3;
__device__ __constant__ const unsigned int DilateApronTwo = 1; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int DilateHaloTwo = 1;  // padding around the center of kernel

__device__ __constant__ const unsigned int DilateTotalOne = 255 * (11 * 11 - 24);
__device__ __constant__ const unsigned int DilateSizeOne = 11;
__device__ __constant__ const unsigned int DilateApronOne = 5; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int DilateHaloOne = 5;  // padding around the center of kernel
		  
__device__ __constant__ const unsigned int ErodeTotalOne = (255 * 21);
__device__ __constant__ const unsigned int ErodeSizeOne = 5;
__device__ __constant__ const unsigned int ErodeApronOne = 2; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int ErodeHaloOne = 2;  // padding around the center of kernel

__device__ __constant__ const unsigned int Guass5x5Total = 273;
__device__ __constant__ const unsigned int Guass5x5Size = 5;
__device__ __constant__ const unsigned int Guass5x5Apron = 2; // padding in image due to kernel boundary
__device__ __constant__ const unsigned int Guass5x5Halo = 2;  // padding around the center of kernel
