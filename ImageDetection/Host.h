//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef HOST_H
#define HOST_H

#include "ImageRGB.h"
#include "ImageGray.h"
#include "Point.h"
#include "Coin.h"

void RGB_to_gray(ImageGray &rImageGrayOut, ImageRGB  &rImageRGBIn);
void threshold(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, unsigned char threshold_val);
void invert(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void erode(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void dilate(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void blur_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void dilate_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void erode_loop(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn, int count);
void find_center(ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints);
void edge_detection(ImageGray& rImageGrayOut, ImageGray& rImageGrayIn);
void label_coins(ImageGray& ImageEdge, ImageRGB& rImageRGBOut, ImageGray& rImageGrayIn, std::vector<Point>& CenterPoints);
void get_coins(PixelGray* pIn, std::vector< Coin >& coins, std::vector<Point>& CenterPoints, unsigned int width, unsigned int height);
void loadCenter(std::vector<Point>& CenterPoints);

void combined(ImageRGB& rImageRGBIn, ImageGray& rImageGrayOut, ImageGray& tmp, ImageGray& tmp_two, std::vector<Point>& CenterPoints, int threshOne, int threshTwo, int blurPasses, int dilatePasses, int erodePasses);
#endif

// --- End of File ---
