#include "kernel.cuh"

int ceiling(int val, int threads)
{
    return (val + threads - 1) / threads;
}

void cudaLoopMemcpy(PixelGray* dst, const void* srcOdd, const void* srcEven, size_t count, int passes)
{
    if (passes % 2 == 1)
    {
        CudaTry(cudaMemcpy(dst, srcOdd, count, cudaMemcpyDeviceToHost));
    }
    else
    {
        CudaTry(cudaMemcpy(dst, srcEven, count, cudaMemcpyDeviceToHost));
    }
}

#if PIPELINE
__device__
#else
__global__
#endif
void RGB_To_Gray_GPU(PixelRGB* rgb, PixelGray* gray, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;


    if (row < h && col < w)
    {
        unsigned int i = row * w + col;
        unsigned int val = (unsigned int)(0.299f * rgb[i].r + 0.587f * rgb[i].g + 0.114f * rgb[i].b);
        gray[i].v = (unsigned char)val;
    }
}

#if PIPELINE
__device__
#else
__global__
#endif
void Threshold_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h, unsigned char threshold)
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

#if PIPELINE
__device__
#else
__global__
#endif
void Invert_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)

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
        else
        {
            out[i].v = 0x0;
        }
    }
}

__global__
void Erode_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int sum_v = 0;
    
    unsigned int k = 0;
    
    if ((row >= ErodeApronOne) && (row < (h - ErodeApronOne)) &&
        (col >= ErodeApronOne) && (col < (w - ErodeApronOne)))
    {
        for (unsigned int i = (row - ErodeHaloOne); i <= (row + ErodeHaloOne); i++)
        {
            for (unsigned int j = (col - ErodeHaloOne); j <= (col + ErodeHaloOne); j++)
            {
                unsigned int index = i * w + j;
    
                sum_v += in[index].v * ErodeKernelOne[k];
    
                k++;
            }
        }
    
    }
    
    if (sum_v == ErodeTotalOne)
    {
        out[row * w + col].v = 255;
    }
    else
    {
        out[row * w + col].v = 0;
    }
}

__global__
void Dilate_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int sum_v = 0;

    unsigned int k = 0;

    if ((row >= DilateApronOne) && (row < (h - DilateApronOne)) &&
        (col >= DilateApronOne) && (col < (w - DilateApronOne)))
    {
        for (unsigned int i = (row - DilateHaloOne); i <= (row + DilateHaloOne); i++)
        {
            for (unsigned int j = (col - DilateHaloOne); j <= (col + DilateHaloOne); j++)
            {
                unsigned int index = i * w + j;

                sum_v += in[index].v * DilateKernelOne[k];

                k++;
            }
        }

    }

    if (sum_v > 255)
    {
        out[row * w + col].v = 255;
    }
    else
    {
        out[row * w + col].v = 0;
    }
}


__global__
void Blur_Loop_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int sum_v = 0;

    unsigned int k = 0;

    if ((row >= Guass5x5Apron) && (row < (h - Guass5x5Apron)) &&
        (col >= Guass5x5Apron) && (col < (w - Guass5x5Apron)))
    {
        for (unsigned int i = (row - Guass5x5Halo); i <= (row + Guass5x5Halo); i++)
        {
            for (unsigned int j = (col - Guass5x5Halo); j <= (col + Guass5x5Halo); j++)
            {
                unsigned int index = i * w + j;

                sum_v += in[index].v * Guass5x5Kernel[k];

                k++;
            }
        }

    }

    out[row * w + col].v = (unsigned char)(sum_v / Guass5x5Total);
}

__global__
void Dilate_Loop_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int sum_v = 0;

    unsigned int k = 0;

    if ((row >= DilateApronTwo) && (row < (h - DilateApronTwo)) &&
        (col >= DilateApronTwo) && (col < (w - DilateApronTwo)))
    {
        for (unsigned int i = (row - DilateHaloTwo); i <= (row + DilateHaloTwo); i++)
        {
            for (unsigned int j = (col - DilateHaloTwo); j <= (col + DilateHaloTwo); j++)
            {
                unsigned int index = i * w + j;

                sum_v += in[index].v * DilateKernelTwo[k];

                k++;
            }
        }

    }

    if (sum_v > 255)
    {
        out[row * w + col].v = 255;
    }
    else
    {
        out[row * w + col].v = 0;
    }
}

__global__
void Erode_Loop_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int sum_v = 0;

    unsigned int k = 0;

    if ((row >= ErodeApronTwo) && (row < (h - ErodeApronTwo)) &&
        (col >= ErodeApronTwo) && (col < (w - ErodeApronTwo)))
    {
        for (unsigned int i = (row - ErodeHaloTwo); i <= (row + ErodeHaloTwo); i++)
        {
            for (unsigned int j = (col - ErodeHaloTwo); j <= (col + ErodeHaloTwo); j++)
            {
                unsigned int index = i * w + j;

                sum_v += in[index].v * ErodeKernelTwo[k];

                k++;
            }
        }

    }

    if (sum_v == ErodeTotalTwo)
    {
        out[row * w + col].v = 255;
    }
    else
    {
        out[row * w + col].v = 0;
    }
}

__global__
void Edge_Detection_GPU(PixelGray* in, PixelGray* out, unsigned int w, unsigned int h)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum_x = 0;
    float sum_y = 0;

    unsigned int k = 0;

    if ((row >= SobelApron) && (row < (h - SobelApron)) &&
        (col >= SobelApron) && (col < (w - SobelApron)))
    {
        for (unsigned int i = (row - SobelHalo); i <= (row + SobelHalo); i++)
        {
            for (unsigned int j = (col - SobelHalo); j <= (col + SobelHalo); j++)
            {
                unsigned int index = i * w + j;

                sum_x += (float)(in[index].v) * SobelKernel_Gx[k];
                sum_y += (float)(in[index].v) * SobelKernel_Gy[k];

                k++;
            }
        }
    }

    float mag = sqrtf(sum_x * sum_x + sum_y * sum_y);

    if (mag > 1.0f)
    {
        out[row * w + col].v = 255;
    }
    else
    {
        out[row * w + col].v = 0;
    }
}

__global__
void Label_Coins_GPU(PixelRGB* out, PixelGray* edge, 
                     Coin* coins, PixelRGB* dimeImg, PixelRGB* pennyImg,
                     PixelRGB* nickleImg, PixelRGB* quarterImg,
                     unsigned int coinSize, unsigned int w, unsigned int h)
{
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;

    for (int n = 0; n < coinSize; n++)
    {
        if (row == coins[n].pt.r && col == coins[n].pt.c)
        {
            unsigned int row_c = coins[n].pt.r;
            unsigned int col_c = coins[n].pt.c;

            if (coins[n].type == Coin::Type::Dime)
            {
                for (unsigned int i = (row_c - 62); i <= (row_c + 62); i++)
                {
                    for (unsigned int j = (col_c - 62); j <= (col_c + 62); j++)
                    {
                        unsigned int index = i * w + j;
                        unsigned int index_label = (i - row_c + 62) * 125 + (j - col_c + 62);

                        out[index].r = dimeImg[index_label].r;
                        out[index].g = dimeImg[index_label].g;
                        out[index].b = dimeImg[index_label].b;
                    }
                }
            }
            else if (coins[n].type == Coin::Type::Penny)
            {
                for (unsigned int i = (row_c - 62); i <= (row_c + 62); i++)
                {
                    for (unsigned int j = (col_c - 62); j <= (col_c + 62); j++)
                    {
                        unsigned int index = i * w + j;
                        unsigned int index_label = (i - row_c + 62) * 125 + (j - col_c + 62);

                        out[index].r = pennyImg[index_label].r;
                        out[index].g = pennyImg[index_label].g;
                        out[index].b = pennyImg[index_label].b;
                    }
                }
            }
            else if (coins[n].type == Coin::Type::Nickel)
            {
                for (unsigned int i = (row_c - 62); i <= (row_c + 62); i++)
                {
                    for (unsigned int j = (col_c - 62); j <= (col_c + 62); j++)
                    {
                        unsigned int index = i * w + j;
                        unsigned int index_label = (i - row_c + 62) * 125 + (j - col_c + 62);

                        out[index].r = nickleImg[index_label].r;
                        out[index].g = nickleImg[index_label].g;
                        out[index].b = nickleImg[index_label].b;
                    }
                }
            }
            else if (coins[n].type == Coin::Type::Quarter)
            {
                for (unsigned int i = (row_c - 62); i <= (row_c + 62); i++)
                {
                    for (unsigned int j = (col_c - 62); j <= (col_c + 62); j++)
                    {
                        unsigned int index = i * w + j;
                        unsigned int index_label = (i - row_c + 62) * 125 + (j - col_c + 62);

                        out[index].r = quarterImg[index_label].r;
                        out[index].g = quarterImg[index_label].g;
                        out[index].b = quarterImg[index_label].b;
                    }
                }
            }
        }
    }

    unsigned int index = row * w + col;

    if (edge[index].v == 255)
    {
        out[index].r = 0;
        out[index].g = 0;
        out[index].b = 250;
    }
}

#if PIPELINE
// figure out how many buffers I need to pass?
__global__ void Pipeline(PixelRGB* rgb, PixelGray* gray, PixelGray* tmp, unsigned int w, unsigned int h, unsigned char threshold)
{
    RGB_To_Gray_GPU(rgb, gray, w, h);
    // it seems like it's faster to modify the array in place.
    Threshold_GPU(gray, gray, w, h, threshold);
    Invert_GPU(gray, gray, w, h);
}

__global__ void ThreshH(PixelGray* gray, unsigned int w, unsigned int h, unsigned char threshold)
{
    Threshold_GPU(gray, gray, w, h, threshold);
}

void combined_CUDA(ImageRGB& rImageRGBIn, ImageGray& rImageGrayOut, ImageGray& tmp, std::vector<Point>& CenterPoints, int threshOne, int threshTwo, int blurPasses, int dilatePasses, int erodePasses)
{
    ImageRGB Image_1;
    ImageRGB Image_5;
    ImageRGB Image_10;
    ImageRGB Image_25;
    Image_1.LoadPngImage("label1.png");
    Image_5.LoadPngImage("label5.png");
    Image_10.LoadPngImage("label10.png");
    Image_25.LoadPngImage("label25.png");

    PixelRGB* in_d;
    PixelGray* out_d;
    PixelGray* tmp_d;
    PixelGray* imagetmp_d;
    Coin* coins_d;
    PixelRGB* dime_d;
    PixelRGB* penny_d;
    PixelRGB* nickle_d;
    PixelRGB* quarter_d;

    size_t RGBLen = sizeof(PixelRGB) * rImageRGBIn.GetPixelCount();
    size_t GrayLen = rImageGrayOut.GetPixelCount();
    size_t Image_1Len = Image_1.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_5Len = Image_5.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_10Len = Image_10.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_25Len = Image_25.GetPixelCount() * sizeof(PixelRGB);
    int w_d = rImageRGBIn.Width();
    int h_d = rImageRGBIn.Height();

    CudaTry(cudaMalloc((void**)&in_d, RGBLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));
    CudaTry(cudaMalloc((void**)&tmp_d, GrayLen));
    CudaTry(cudaMalloc((void**)&imagetmp_d, GrayLen));
    CudaTry(cudaMalloc((void**)&penny_d, Image_1Len));
    CudaTry(cudaMalloc((void**)&nickle_d, Image_5Len));
    CudaTry(cudaMalloc((void**)&dime_d, Image_10Len));
    CudaTry(cudaMalloc((void**)&quarter_d, Image_25Len));

    CudaTry(cudaMemcpy(in_d, rImageRGBIn.GetPixelRGB(), RGBLen, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(penny_d, Image_1.GetPixelRGB(), Image_1Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(nickle_d, Image_5.GetPixelRGB(), Image_5Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(dime_d, Image_10.GetPixelRGB(), Image_10Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(quarter_d, Image_25.GetPixelRGB(), Image_25Len, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Pipeline << <Grid, Block >> > (in_d, out_d, tmp_d, w_d, h_d, threshOne);
    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    Erode_GPU << <Grid, Block >> > (out_d, tmp_d, w_d, h_d);
    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    Dilate_GPU << <Grid, Block >> > (tmp_d, out_d, w_d, h_d);
    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    for (int j = 0; j < blurPasses; j++)
    {
        Blur_Loop_GPU << < Grid, Block >> > (out_d, tmp_d, w_d, h_d);
        
        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = out_d;
        out_d = tmp_d;
        tmp_d = pTmp;
    }

    ThreshH << <Grid, Block >> > (out_d, w_d, h_d, threshTwo);

    for (int j = 0; j < dilatePasses; j++)
    {
        Dilate_Loop_GPU << < Grid, Block >> > (out_d, tmp_d, w_d, h_d);

        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = out_d;
        out_d = tmp_d;
        tmp_d = pTmp;
    }

    CudaTry(cudaMemcpy(imagetmp_d, tmp_d, GrayLen, cudaMemcpyDeviceToDevice));

    for (int j = 0; j < erodePasses; j++)
    {
        Erode_Loop_GPU << < Grid, Block >> > (tmp_d, out_d, w_d, h_d);

        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = out_d;
        out_d = tmp_d;
        tmp_d = pTmp;
    }

    CudaTry(cudaMemcpy(tmp.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));
   
    find_center(tmp, CenterPoints);

	Edge_Detection_GPU << <Grid, Block >> > (imagetmp_d, tmp_d, w_d, h_d);

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), tmp_d, GrayLen, cudaMemcpyDeviceToHost));

    std::vector<Coin> coins;
    get_coins(rImageGrayOut.GetPixelGray(), coins, CenterPoints, w_d, h_d);

    unsigned int coinSize_d = coins.size();
    size_t CoinLen = coinSize_d * sizeof(Coin);

    CudaTry(cudaMalloc((void**)&coins_d, CoinLen));
    CudaTry(cudaMemcpy(coins_d, coins.data(), CoinLen, cudaMemcpyHostToDevice));

	Label_Coins_GPU << <Grid, Block >> > (in_d, tmp_d, coins_d, dime_d, penny_d, nickle_d, quarter_d, coinSize_d, w_d, h_d);

    CudaTry(cudaMemcpy(rImageRGBIn.GetPixelRGB(), in_d, RGBLen, cudaMemcpyDeviceToHost));

    CudaTry(cudaFree(in_d));
    CudaTry(cudaFree(out_d));
    CudaTry(cudaFree(tmp_d));
    CudaTry(cudaFree(imagetmp_d));
    CudaTry(cudaFree(coins_d));
    CudaTry(cudaFree(dime_d));
    CudaTry(cudaFree(penny_d));
    CudaTry(cudaFree(nickle_d));
    CudaTry(cudaFree(quarter_d));
}

#else 

void RGB_to_gray_CUDA(ImageGray& rImageGrayOut, ImageRGB& rImageRGBIn)
{
    PixelRGB* in_d;
    PixelGray* out_d;
    int w_d = rImageRGBIn.Width();
    int h_d = rImageRGBIn.Height();

    size_t RGBLen = sizeof(PixelRGB) * rImageRGBIn.GetPixelCount();
    size_t GrayLen = rImageGrayOut.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, RGBLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageRGBIn.GetPixelRGB(), RGBLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    RGB_To_Gray_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void threshold_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char threshold)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Threshold_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d, threshold);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void invert_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Invert_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void erode_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Erode_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void dilate_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();


    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Dilate_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));


    cudaFree(in_d);
    cudaFree(out_d);
}

void blur_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();

    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    for (int n = 0; n < count; n++)
    {
        Blur_Loop_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = in_d;
        in_d = out_d;
        out_d = pTmp;
    }
    if (count % 2 == 1)
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), in_d, GrayLen, cudaMemcpyDeviceToHost));
    }
    else
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));

    }

    cudaFree(in_d);
    cudaFree(out_d);
}

void dilate_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();

    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    for (int n = 0; n < count; n++)
    {
        Dilate_Loop_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = in_d;
        in_d = out_d;
        out_d = pTmp;
    }

    if (count % 2 == 1)
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), in_d, GrayLen, cudaMemcpyDeviceToHost));
    }
    else
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));

    }

    cudaFree(in_d);
    cudaFree(out_d);
}

void erode_loop_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();

    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));


    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    for (int n = 0; n < count; n++)
    {
        Erode_Loop_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

        CudaTry(cudaDeviceSynchronize());

        PixelGray* pTmp = in_d;
        in_d = out_d;
        out_d = pTmp;
    }

    if (count % 2 == 1)
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), in_d, GrayLen, cudaMemcpyDeviceToHost));
    }
    else
    {
        CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));
    }

    cudaFree(in_d);
    cudaFree(out_d);
}

void edge_detection_CUDA(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
    PixelGray* in_d;
    PixelGray* out_d;
    int w_d = rImageGrayIn.Width();
    int h_d = rImageGrayIn.Height();

    size_t GrayLen = rImageGrayIn.GetPixelCount();

    CudaTry(cudaMalloc((void**)&in_d, GrayLen));
    CudaTry(cudaMalloc((void**)&out_d, GrayLen));

    CudaTry(cudaMemcpy(in_d, rImageGrayIn.GetPixelGray(), GrayLen, cudaMemcpyHostToDevice));


    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    
    Edge_Detection_GPU << <Grid, Block >> > (in_d, out_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageGrayOut.GetPixelGray(), out_d, GrayLen, cudaMemcpyDeviceToHost));
    cudaFree(in_d);
    cudaFree(out_d);
}

void label_coins_CUDA(ImageGray& ImageEdge, ImageRGB& rImageRGBOut, ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints)
{
    unsigned int w_d = rImageGrayIn.Width();
    unsigned int h_d = rImageGrayIn.Height();

    PixelRGB* out_d;
    PixelGray* edge_d;

    Coin* coins_d;
    PixelRGB* dime_d;
    PixelRGB* penny_d;
    PixelRGB* nickle_d;
    PixelRGB* quarter_d;

    ImageRGB Image_1;
    ImageRGB Image_5;
    ImageRGB Image_10;
    ImageRGB Image_25;

    Image_1.LoadPngImage("label1.png");
    Image_5.LoadPngImage("label5.png");
    Image_10.LoadPngImage("label10.png");
    Image_25.LoadPngImage("label25.png");

    std::vector<Coin> coins;
    get_coins(rImageGrayIn.GetPixelGray(), coins, CenterPoints, w_d, h_d);

    unsigned int coinSize_d = coins.size();

    size_t RGBLen = rImageRGBOut.GetPixelCount() * sizeof(PixelRGB);
    size_t EdgeLen = ImageEdge.GetPixelCount();
    size_t CoinLen = coinSize_d * sizeof(Coin);

    size_t Image_1Len = Image_1.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_5Len = Image_1.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_10Len = Image_1.GetPixelCount() * sizeof(PixelRGB);
    size_t Image_25Len = Image_1.GetPixelCount() * sizeof(PixelRGB);

    CudaTry(cudaMalloc((void**)&out_d, RGBLen));
    CudaTry(cudaMalloc((void**)&edge_d, EdgeLen));
    CudaTry(cudaMalloc((void**)&coins_d, CoinLen));
    CudaTry(cudaMalloc((void**)&penny_d, Image_1Len));
    CudaTry(cudaMalloc((void**)&nickle_d, Image_5Len));
    CudaTry(cudaMalloc((void**)&dime_d, Image_10Len));
    CudaTry(cudaMalloc((void**)&quarter_d, Image_25Len));

    CudaTry(cudaMemcpy(out_d, rImageRGBOut.GetPixelRGB(), RGBLen, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(edge_d, ImageEdge.GetPixelGray(), EdgeLen, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(coins_d, coins.data(), CoinLen, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(penny_d, Image_1.GetPixelRGB(), Image_1Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(nickle_d, Image_5.GetPixelRGB(), Image_5Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(dime_d, Image_10.GetPixelRGB(), Image_10Len, cudaMemcpyHostToDevice));
    CudaTry(cudaMemcpy(quarter_d, Image_25.GetPixelRGB(), Image_25Len, cudaMemcpyHostToDevice));

    int blocksX = ceiling(w_d, THREAD_X);
    int blocksY = ceiling(h_d, THREAD_Y);
    dim3 Block(THREAD_X, THREAD_Y, 1);
    dim3 Grid(blocksX, blocksY, 1);

    Label_Coins_GPU << <Grid, Block >> > (out_d, edge_d, coins_d, dime_d, penny_d, nickle_d, quarter_d, coinSize_d, w_d, h_d);

    CudaTry(cudaGetLastError());
    CudaTry(cudaDeviceSynchronize());

    CudaTry(cudaMemcpy(rImageRGBOut.GetPixelRGB(), out_d, RGBLen, cudaMemcpyDeviceToHost));

    CudaTry(cudaFree(out_d));
    CudaTry(cudaFree(edge_d));
    CudaTry(cudaFree(coins_d));
    CudaTry(cudaFree(dime_d));
    CudaTry(cudaFree(penny_d));
    CudaTry(cudaFree(nickle_d));
    CudaTry(cudaFree(quarter_d));
}

#endif