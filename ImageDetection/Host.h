//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef HOST_H
#define HOST_H

#include "ImageRGB.h"
#include "ImageGray.h"

void Host_ImageProcess_one(ImageGray &rImageGrayOut, ImageRGB  &rImageRGBIn);
void Host_ImageProcess_two(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char Threshold);

#endif

// --- End of File ---
