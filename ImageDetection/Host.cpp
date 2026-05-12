//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "Host.h"

void Host_ImageProcess(ImageGray &rImageGrayOut, ImageRGB &rImageRGBIn)
{
	unsigned int width = rImageRGBIn.Width();
	unsigned int height = rImageRGBIn.Height();

	PixelRGB *pPixelRGB = rImageRGBIn.GetPixelRGB();
	PixelGray *pPixelGray = rImageGrayOut.GetPixelGray();

	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			unsigned int index = row * width + col;

			unsigned int val = (unsigned int)(0.299f * pPixelRGB[index].r + 0.587f * pPixelRGB[index].g + 0.114f * pPixelRGB[index].b);

			pPixelGray[index].v = (unsigned char)val;
		}
	}
}

// --- End of File ---
