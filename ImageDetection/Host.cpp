//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------
#include "Host.h"
#include "kernel.cuh"

enum Dir
{
	North,
	South,
	East,
	West,
	size
};

void SortMe(int* arr)
{
	if (arr[0] > arr[1])
	{
		int temp = arr[0];
		arr[0] = arr[1];
		arr[1] = temp;
	}

	if (arr[1] > arr[2])
	{
		int temp = arr[1];
		arr[1] = arr[2];
		arr[2] = temp;
	}

	if (arr[2] > arr[3])
	{
		int temp = arr[2];
		arr[2] = arr[3];
		arr[3] = temp;
	}
}
void RGB_to_gray(ImageGray &rImageGrayOut, ImageRGB &rImageRGBIn)
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

void threshold(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char threshold_val)
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

			if (pPixelIn[index].v > threshold_val)
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

void invert(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
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

void erode(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();
	PixelGray* pPixelOut = rImageGrayOut.GetPixelGray();

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			unsigned int sum_v = 0;

			unsigned int k = 0;

			if ((row >= ErodeApronOne) && (row < (height - ErodeApronOne)) &&
				(col >= ErodeApronOne) && (col < (width - ErodeApronOne)))
			{
				for (unsigned int i = (row - ErodeHaloOne); i <= (row + ErodeHaloOne); i++)
				{
					for (unsigned int j = (col - ErodeHaloOne); j <= (col + ErodeHaloOne); j++)
					{
						unsigned int index = i * width + j;

						sum_v += pPixelIn[index].v * ErodeKernelOne[k];

						k++;
					}
				}
			}

			if (sum_v == ErodeTotalOne)
			{
				pPixelOut[row * width + col].v = 255;
			}
			else
			{
				pPixelOut[row * width + col].v = 0;
			}
		}
	}
}

void dilate(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();
	PixelGray* pPixelOut = rImageGrayOut.GetPixelGray();
	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			unsigned int sum_v = 0;

			unsigned int k = 0;

			if ((row >= DilateApronOne) && (row < (height - DilateApronOne)) &&
				(col >= DilateApronOne) && (col < (width - DilateApronOne)))
			{
				for (unsigned int i = (row - DilateApronOne); i <= (row + DilateApronOne); i++)
				{
					for (unsigned int j = (col - DilateApronOne); j <= (col + DilateApronOne); j++)
					{
						unsigned int index = i * width + j;

						sum_v += pPixelIn[index].v * DilateKernelOne[k];

						k++;
					}
				}
			}

			if (sum_v > 255)
			{
				pPixelOut[row * width + col].v = 255;
			}
			else
			{
				pPixelOut[row * width + col].v = 0;
			}
		}
	}
}

void blur_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayOut.Height();

	PixelGray* pA = rImageGrayIn.GetPixelGray();
	PixelGray* pB = rImageGrayOut.GetPixelGray();

	for (int n = 0; n < count; n++)
	{
		for (unsigned int row = 0; row < height; row++)
		{
			for (unsigned int col = 0; col < width; col++)
			{
				unsigned int sum_v = 0;

				unsigned int k = 0;

				if ((row >= Guass5x5Apron) && (row < (height - Guass5x5Apron)) &&
					(col >= Guass5x5Apron) && (col < (width - Guass5x5Apron)))
				{
					for (unsigned int i = (row - Guass5x5Halo); i <= (row + Guass5x5Halo); i++)
					{
						for (unsigned int j = (col - Guass5x5Halo); j <= (col + Guass5x5Halo); j++)
						{
							unsigned int index = i * width + j;

							sum_v += pA[index].v * Guass5x5Kernel[k];

							k++;
						}
					}

				}

				pB[row * width + col].v = (unsigned char)(sum_v / Guass5x5Total);

			}
		}

		PixelGray* pTmp = pA;
		pA = pB;
		pB = pTmp;
	}
}


void dilate_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pA = rImageGrayIn.GetPixelGray();
	PixelGray* pB = rImageGrayOut.GetPixelGray();

	for (int n = 0; n < count; n++)
	{
		for (unsigned int row = 0; row < height; row++)
		{
			for (unsigned int col = 0; col < width; col++)
			{
				unsigned int sum_v = 0;

				unsigned int k = 0;

				if ((row >= DilateApronTwo) && (row < (height - DilateApronTwo)) &&
					(col >= DilateApronTwo) && (col < (width - DilateApronTwo)))
				{
					for (unsigned int i = (row - DilateHaloTwo); i <= (row + DilateHaloTwo); i++)
					{
						for (unsigned int j = (col - DilateHaloTwo); j <= (col + DilateHaloTwo); j++)
						{
							unsigned int index = i * width + j;

							sum_v += pA[index].v * DilateKernelTwo[k];

							k++;
						}
					}

				}

				if (sum_v > 255)
				{
					pB[row * width + col].v = 255;
				}
				else
				{
					pB[row * width + col].v = 0;
				}
			}
		}

		PixelGray* pTmp = pA;
		pA = pB;
		pB = pTmp;
	}
}

void erode_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count)
{
	PixelGray* pA = rImageGrayIn.GetPixelGray();
	PixelGray* pB = rImageGrayOut.GetPixelGray();

	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	for (int n = 0; n < count; n++)
	{
		for (unsigned int row = 0; row < height; row++)
		{
			for (unsigned int col = 0; col < width; col++)
			{
				unsigned int sum_v = 0;
				unsigned int k = 0;

				if ((row >= ErodeApronTwo) && (row < (height - ErodeApronTwo)) &&
					(col >= ErodeApronTwo) && (col < (width - ErodeApronTwo)))
				{
					for (unsigned int i = (row - ErodeHaloTwo); i <= (row + ErodeHaloTwo); i++)
					{
						for (unsigned int j = (col - ErodeHaloTwo); j <= (col + ErodeHaloTwo); j++)
						{
							unsigned int index = i * width + j;

							sum_v += pA[index].v * ErodeKernelTwo[k];

							k++;
						}
					}
				}

				if (sum_v == ErodeTotalTwo)
				{
					pB[row * width + col].v = 255;
				}
				else
				{
					pB[row * width + col].v = 0;
				}
			}
		}

		PixelGray* pTmp = pA;
		pA = pB;
		pB = pTmp;
	}
}

void find_center(ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints)
{
	// We do a quick estimate for the blob
	// We might be off a few pixel
	// Clear the blob with a little extra size
	unsigned int slop = 40;

	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();

	Point Top;
	Point Bot;
	Point Center;
	Point Left;
	Point Right;

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			// Find Top
			if (pPixelIn[row * width + col].v > 0)
			{
				Top.r = row;
				Top.c = col;

				// Find Bottom point
				for (unsigned int rowA = row; rowA < height; rowA++)
				{
					if (pPixelIn[rowA * width + col].v < 255)
					{
						Bot.r = rowA - 1;
						Bot.c = col;
						break;
					}
				}

				// Find the center estimate (half way between Top and Bottom)
				Center.r = Top.r + (Bot.r - Top.r) / 2;
				Center.c = Top.c;

				// Find Left
				for (unsigned int colA = Center.c; colA > 0; colA--)
				{
					if (pPixelIn[Center.r * width + colA].v < 255)
					{
						Left.r = Center.r;
						Left.c = colA + 1;
						break;
					}
				}

				// Find Right
				for (unsigned int colB = Center.c; colB < width; colB++)
				{
					if (pPixelIn[Center.r * width + colB].v < 255)
					{
						Right.r = Center.r;
						Right.c = colB - 1;
						break;
					}
				}

				// Output the center
				CenterPoints.push_back(Center);

				// Zero the blob Box
				for (unsigned int rowC = Top.r - slop; rowC < Bot.r + slop; rowC++)
				{
					for (unsigned int colC = Left.c - slop; colC < Right.c + slop; colC++)
					{
						pPixelIn[rowC * width + colC].v = 0;
					}
				}

				break;
			}

		}
	}
}

void edge_detection(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pPixelIn = rImageGrayIn.GetPixelGray();
	PixelGray* pPixelOut = rImageGrayOut.GetPixelGray();

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			float sum_x = 0;
			float sum_y = 0;

			unsigned int k = 0;

			if ((row >= SobelApron) && (row < (height - SobelApron)) &&
				(col >= SobelApron) && (col < (width - SobelApron)))
			{
				for (unsigned int i = (row - SobelHalo); i <= (row + SobelHalo); i++)
				{
					for (unsigned int j = (col - SobelHalo); j <= (col + SobelHalo); j++)
					{
						unsigned int index = i * width + j;

						sum_x += (float)(pPixelIn[index].v) * SobelKernel_Gx[k];
						sum_y += (float)(pPixelIn[index].v) * SobelKernel_Gy[k];

						k++;
					}
				}
			}

			float mag = sqrtf(sum_x * sum_x + sum_y * sum_y);

			if (mag > 1.0f)
			{
				pPixelOut[row * width + col].v = 255;
			}
			else
			{
				pPixelOut[row * width + col].v = 0;
			}
		}
	}
}

void label_coins(ImageGray& ImageEdge, ImageRGB& rImageRGBOut, ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints)
{
	unsigned int width = rImageGrayIn.Width();
	unsigned int height = rImageGrayIn.Height();

	PixelGray* pIn = rImageGrayIn.GetPixelGray();
	PixelRGB* pOut = rImageRGBOut.GetPixelRGB();
	PixelGray* pEdge = ImageEdge.GetPixelGray();

	std::vector<Coin> coins;

	get_coins(pIn, coins, CenterPoints, width, height);

	ImageRGB Image_1;
	Image_1.LoadPngImage("label1.png");
	PixelRGB* pImage1 = Image_1.GetPixelRGB();

	ImageRGB Image_5;
	Image_5.LoadPngImage("label5.png");
	PixelRGB* pImage5 = Image_5.GetPixelRGB();

	ImageRGB Image_10;
	Image_10.LoadPngImage("label10.png");
	PixelRGB* pImage10 = Image_10.GetPixelRGB();

	ImageRGB Image_25;
	Image_25.LoadPngImage("label25.png");
	PixelRGB* pImage25 = Image_25.GetPixelRGB();

	for (int n = 0; n < coins.size(); n++)
	{
		//unsigned int index = row * width + col;

		unsigned int row = coins[n].pt.r;
		unsigned int col = coins[n].pt.c;

		PixelRGB* pLabel = nullptr;

		if (coins[n].type == Coin::Type::Dime)
		{
			pLabel = pImage10;
		}
		else if (coins[n].type == Coin::Type::Penny)
		{
			pLabel = pImage1;
		}
		else if (coins[n].type == Coin::Type::Nickel)
		{
			pLabel = pImage5;
		}
		else if (coins[n].type == Coin::Type::Quarter)
		{
			pLabel = pImage25;
		}

		for (unsigned int i = (row - 62); i <= (row + 62); i++)
		{
			for (unsigned int j = (col - 62); j <= (col + 62); j++)
			{
				unsigned int index = i * width + j;
				unsigned int index_label = (i - row + 62) * 125 + (j - col + 62);

				pOut[index].r = pLabel[index_label].r;
				pOut[index].g = pLabel[index_label].g;
				pOut[index].b = pLabel[index_label].b;


			}
		}
	}


	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			unsigned int index = row * width + col;

			if (pEdge[index].v == 255)
			{
				pOut[index].r = 0;
				pOut[index].g = 0;
				pOut[index].b = 250;
			}

		}
	}
}

void get_coins(PixelGray* pIn, std::vector< Coin >& coins, std::vector<Point>& CenterPoints, unsigned int width, unsigned int height)
{
	for (size_t i = 0; i < CenterPoints.size(); i++)
	{
		unsigned int row_st = CenterPoints[i].r;
		unsigned int col_st = CenterPoints[i].c;

		int radius[Dir::size]{ 0 };

		unsigned int row;
		unsigned int col;

		row = row_st;
		col = col_st;
		for (unsigned int rowA = row; rowA >= 0; rowA--)
		{
			if (pIn[rowA * width + col].v > 0)
			{
				break;
			}
			else
			{
				radius[Dir::North]++;
			}
		}

		row = row_st;
		col = col_st;
		for (unsigned int rowB = row; rowB < height; rowB++)
		{
			if (pIn[rowB * width + col].v > 0)
			{
				break;
			}
			else
			{
				radius[Dir::South]++;
			}
		}

		row = row_st;
		col = col_st;
		for (unsigned int colA = col; colA >= 0; colA--)
		{
			if (pIn[row * width + colA].v > 0)
			{
				break;
			}
			else
			{
				radius[Dir::West]++;
			}
		}

		row = row_st;
		col = col_st;
		for (unsigned int colB = col; colB < width; colB++)
		{
			if (pIn[row * width + colB].v > 0)
			{
				break;
			}
			else
			{
				radius[Dir::East]++;
			}
		}

		SortMe(radius);

		float effective_diameter = (float)(radius[0]) + (float)radius[1];
		Coin tmp;
		tmp.Set(CenterPoints[i], effective_diameter);

		coins.push_back(tmp);
	}

	for (size_t n = 0; n < coins.size(); n++)
	{
		Trace::out("[%2d]: --------------------\n", n);
		coins[n].Print();

	}
}
void loadCenter(std::vector<Point>& CenterPoints)
{
	FILE* fh;
	fopen_s(&fh, "10_CenterPoints.bin", "r");
	fseek(fh, 0, SEEK_END);
	size_t size = ftell(fh);
	fseek(fh, 0, SEEK_SET);
	CenterPoints.resize(size / sizeof(Point));
	fread(CenterPoints.data(), size, 1, fh);
	fclose(fh);

	//for(size_t i = 0; i < CenterPoints.size(); i++)
	//{
	//	Trace::out("[%2d]: r:%d c:%d \n", i, CenterPoints[i].r, CenterPoints[i].c);
	//}
	//Trace::out("\n");
}

void store(ImageGray& out, ImageGray& in)
{
	PixelGray* outp = out.GetPixelGray();
	PixelGray* inp = in.GetPixelGray();

	for (int i = 0; i < in.GetPixelCount(); i++)
	{
		outp[i].v = inp[i].v;
	}
}

void combined(ImageRGB& rImageRGBIn, ImageGray& rImageGrayOut, ImageGray& tmp, ImageGray& tmp_two, std::vector<Point>& CenterPoints, int threshOne, int threshTwo, int blurPasses, int dilatePasses, int erodePasses)
{
	RGB_to_gray(rImageGrayOut, rImageRGBIn);

	threshold(rImageGrayOut, rImageGrayOut, threshOne);

	invert(rImageGrayOut, rImageGrayOut);

	erode(tmp, rImageGrayOut);

	dilate(rImageGrayOut, tmp);

	blur_loop(tmp, rImageGrayOut, blurPasses);

	threshold(tmp, tmp, threshTwo);

	dilate_loop(rImageGrayOut, tmp, dilatePasses);

	store(tmp_two, rImageGrayOut);

	erode_loop(tmp, rImageGrayOut, erodePasses);

	find_center(tmp, CenterPoints);

	edge_detection(rImageGrayOut, tmp_two);

	label_coins(rImageGrayOut, rImageRGBIn, rImageGrayOut, CenterPoints);
}

// --- End of File ---
