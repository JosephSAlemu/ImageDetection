#include "kernel.cuh"
#include "CudaFramework.h"

__global__ void RGB_To_Gray(PixelRGB* rgb, PixelGray* gray, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int i = row * w + col;

    if (row < h && col < w)
    {
        unsigned int val = (unsigned int)(0.299f * rgb[i].r + 0.587f * rgb[i].g + 0.114f * rgb[i].b);
        gray[i].v = (unsigned char)val;
    }
}

__global__ void Gray_Threshold(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h, unsigned char threshold)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;


    if (row < h && col < w)
    {
        unsigned int i = row * w + col;

        if (in[i].v > threshold)
        {
            out[i].v = 0xFF;
        }
        else
        {
            out[i].v = 0x0;
        }
    }
}

__global__ void Gray_Invert(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < h && col < w)
    {
        unsigned int i = row * w + col;

        if (in[i].v == 0)
        {
            out[i].v = 0xFF;
        }
    }
}

int cieling(int val, int threads)
{
    return (val + threads - 1) / threads;
}

void Device_ImageProcess_one(ImageGray& rImageGrayOut, ImageRGB& rImageRGBIn)
{
    PixelRGB*  in_d;
    PixelGray* out_d;
    int w_d = rImageRGBIn.Width();
    int h_d = rImageRGBIn.Height();

    size_t RGBLen = sizeof(PixelRGB) * rImageRGBIn.GetPixelCount();
    size_t GrayLen = rImageRGBIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, RGBLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageRGBIn.GetPixelRGB(), RGBLen, cudaMemcpyHostToDevice));
    
    int threads = 32;
    int blocksX = cieling(w_d, threads);
    int blocksY = cieling(h_d, threads);
    dim3 Block(threads, threads, 1);
    dim3 Grid(blocksX, blocksY, 1);
    RGB_To_Gray << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void Device_ImageProcess_two(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char Threshold)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int threads = 32;
    int blocksX = cieling(w_d, threads);
    int blocksY = cieling(h_d, threads);
    dim3 Block(threads, threads, 1);
    dim3 Grid(blocksX, blocksY, 1);
    Gray_Threshold << <Grid, Block >> > (in_d, out_d, w_d, h_d, Threshold);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void Device_ImageProcess_three(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int threads = 32;
    int blocksX = cieling(w_d, threads);
    int blocksY = cieling(h_d, threads);
    dim3 Block(threads, threads, 1);
    dim3 Grid(blocksX, blocksY, 1);
    Gray_Invert << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}