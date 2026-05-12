//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "ImageGray.h"

ImageGray::ImageGray()
	:
	poRawImage(nullptr),
	pPixelGray(nullptr),
	mWidth(0),
	mHeight(0),
	mPixelCount(0),
	mState(),
	poFileBuffer(nullptr)

{
	this->poRawImage = new PixelGrayStorage();
	assert(this->poRawImage);

	this->poFileBuffer = new FileStorage();
	assert(this->poFileBuffer);
}

void ImageGray::InitializeEmpty(unsigned int width, unsigned int height)
{
	this->mHeight = height;
	this->mWidth = width;
	this->SetPixelCount(this->mWidth, this->mHeight);

	this->poRawImage->reserve(this->mPixelCount);

	for(size_t i = 0; i < this->mPixelCount; i++)
	{
		this->poRawImage->push_back(0);
	}
}

ImageGray::~ImageGray()
{
	delete this->poRawImage;
	this->poRawImage = nullptr;

	delete this->poFileBuffer;
	this->poFileBuffer = nullptr;
}

lodepng::State &ImageGray::GetState()
{
	return this->mState;
}

PixelGrayStorage &ImageGray::GetPixelStorage()
{
	return *this->poRawImage;
}

FileStorage &ImageGray::GetFileStorage()
{
	return *this->poFileBuffer;
}

PixelGray *ImageGray::GetPixelGray()
{
	assert(this->poRawImage);

	this->pPixelGray = (PixelGray *)&(*this->poRawImage)[0];
	assert(this->pPixelGray);

	return this->pPixelGray;
}

unsigned int &ImageGray::Width()
{
	return this->mWidth;
}

unsigned int &ImageGray::Height()
{
	return this->mHeight;
}

unsigned int ImageGray::GetPixelCount() const
{
	return this->mPixelCount;
}

void ImageGray::SetPixelCount(unsigned int width, unsigned int height)
{
	this->mPixelCount = width * height;
}

void ImageGray::LoadPngImage(const char *pFileName)
{
	assert(pFileName);
	unsigned int error;

	// Initialize state
	this->GetState().decoder.color_convert = 1;
	this->GetState().decoder.remember_unknown_chunks = 0;

	// Load file
	error = lodepng::load_file(this->GetFileStorage(), pFileName);
	assert(!error);

	// Decode file
	error = lodepng::decode(this->GetPixelStorage(),
							this->Width(),
							this->Height(),
							std::string(pFileName),
							LodePNGColorType::LCT_GREY,
							8);
	assert(!error);

	this->SetPixelCount(this->Width(), this->Height());

	Trace::out("Read image: \n");
	Trace::out("	      File: %s\n", pFileName);
	Trace::out("	     Width: %d \n", this->Width());
	Trace::out("	    Height: %d \n", this->Height());
	Trace::out("\n");
}

void ImageGray::WritePngImage(const char *pFileName)
{
	assert(pFileName);
	unsigned int error;

	this->GetFileStorage().erase(std::begin(this->GetFileStorage()), std::end(this->GetFileStorage()));

	error = lodepng_encode_file(pFileName,
								(const unsigned char *)this->GetPixelGray(),
								this->Width(),
								this->Height(),
								LodePNGColorType::LCT_GREY,
								8);

	assert(!error);

	Trace::out("Write image: \n");
	Trace::out("	      File: %s\n", pFileName);
	Trace::out("	     Width: %d \n", this->Width());
	Trace::out("	    Height: %d \n", this->Height());
	Trace::out("\n");
}

// --- End of File ---
