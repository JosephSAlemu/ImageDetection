//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef IMAGE_RGB_H
#define IMAGE_RGB_H

#include "PixelRGB.h"
#include "lodepng.h"

typedef std::vector<unsigned char> PixelRGBStorage;
typedef std::vector<unsigned char> FileStorage;

struct ImageRGB
{
	ImageRGB();
	ImageRGB(const ImageRGB &) = delete;
	ImageRGB &operator=(const ImageRGB &) = delete;
	~ImageRGB();

	PixelRGBStorage &GetPixelStorage();
	PixelRGB *GetPixelRGB();
	unsigned int &Width();
	unsigned int &Height();

	void LoadPngImage(const char *pFileName);
	void WritePngImage(const char *pFileName);

	unsigned int GetPixelCount() const;
	void SetPixelCount(unsigned int width, unsigned int height);

	FileStorage &GetFileStorage();
	lodepng::State &GetState();

	// -------------------------------
	// Data
	// -------------------------------
private:

	PixelRGBStorage  *poRawImage;
	PixelRGB         *pPixelRGB;
	unsigned int     mWidth;
	unsigned int     mHeight;
	unsigned int     mPixelCount;

	lodepng::State   mState;
	FileStorage       *poFileBuffer;
};

#endif

// ---- End of File ---
