#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include "ImageGray.h"
#include "ImageRGB.h"
void Device_ImageProcess_one(ImageGray& rImageGrayOut, ImageRGB& rImageRGBIn);
void Device_ImageProcess_two(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char Threshold);