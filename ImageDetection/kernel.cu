#include "kernel.cuh"
#include "CudaFramework.h"

__global__ void RGB_To_Gray(PixelRGB* in, PixelGray* out, unsigned int w, unsigned int h)
{

}


cudaError_t Device_ImageProcess(ImageRGB& rImageRGBIn, ImageGray& rImageGrayOut)
{
    PixelRGB*  in_d;
    PixelGray* out_d;
    int w_d = rImageRGBIn.Width();
    int h_d = rImageRGBIn.Height();

    size_t RGBLen = sizeof(PixelRGB) * rImageRGBIn.GetPixelCount();
    size_t GrayLen = sizeof(PixelGray) * rImageRGBIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, RGBLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageRGBIn.GetPixelRGB(), RGBLen, cudaMemcpyHostToDevice));
    
    int threads = 32;
    int blocksX = w_d / threads;
    int blocksY = h_d / threads;
    dim3 Block(threads, threads, 1);
    dim3 Grid(blocksX, blocksY, 1);
    RGB_To_Gray << <Grid, Block >> > (in_d, out_d, w, h);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.pPixelGray, out_d, GrayLen, cudaMemcpyHostToDevice));


    cudaFree(in_d);
    cudaFree(out_d);
}
