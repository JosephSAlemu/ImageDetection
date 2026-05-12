#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include "ImageGray.h"
#include "ImageRGB.h"
void Device_ImageProcess(ImageGray& rImageGrayOut, ImageRGB& rImageRGBIn);