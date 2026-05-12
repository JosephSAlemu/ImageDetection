//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "Host.h"

void Host_ImageProcess_one(ImageGray &rImageGrayOut, ImageRGB &rImageRGBIn)
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

void Host_ImageProcess_two(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char Threshold)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();
	PixelGray* pPixelOut = rImageGrayOut.GetPixelGray();

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			unsigned int index = row * width + col;

			if (pPixelIn[index].v > Threshold)
			{
				pPixelOut[index].v = 0xFF;
			}
			else
			{
				pPixelOut[index].v = 0x0;
			}

		}
	}
}


void Host_ImageProcess_three(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();
	PixelGray* pPixelOut = rImageGrayOut.GetPixelGray();

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			unsigned int index = row * width + col;

			if (pPixelIn[index].v == 0)
			{
				pPixelOut[index].v = 0xFF;
			}
			else
			{
				pPixelOut[index].v = 0x0;
			}


		}
	}
}

// --- End of File ---
