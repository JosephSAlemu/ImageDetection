//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#ifndef IMAGE_GRAY_H
#define IMAGE_GRAY_H

#include "PixelGray.h"
#include "lodepng.h"

typedef std::vector<unsigned char> PixelGrayStorage;
typedef std::vector<unsigned char> FileStorage;

struct ImageGray
{
	ImageGray();
	ImageGray(const ImageGray &) = delete;
	ImageGray &operator=(const ImageGray &) = delete;
	~ImageGray();

	void InitializeEmpty(unsigned int width, unsigned int height);

	PixelGrayStorage &GetPixelStorage();
	PixelGray *GetPixelGray();
	unsigned int &Width();
	unsigned int &Height();
	FileStorage &GetFileStorage();
	lodepng::State &GetState();


	void LoadPngImage(const char *pFileName);
	void WritePngImage(const char *pFileName);

	unsigned int GetPixelCount() const;
	void SetPixelCount(unsigned int width, unsigned int height);

	// -------------------------------
	// Data
	// -------------------------------

	// Public so I can CudaMemCpy.
	PixelGray* pPixelGray;

private:

	PixelGrayStorage *poRawImage;
	unsigned int     mWidth;
	unsigned int     mHeight;
	unsigned int     mPixelCount;

	lodepng::State   mState;
	FileStorage     *poFileBuffer;
};

#endif

// ---- End of File ---
