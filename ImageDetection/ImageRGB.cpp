//------------------------------------------------------------
// Copyright 2026, Ed Keenan, all rights reserved.
//------------------------------------------------------------

#include "ImageRGB.h"

ImageRGB::ImageRGB()
	:
	poRawImage(nullptr),
	pPixelRGB(nullptr),
	mWidth(0),
	mHeight(0),
	mPixelCount(0),
	mState(),
	poFileBuffer(nullptr)

{
	this->poRawImage = new PixelRGBStorage();
	assert(this->poRawImage);

	this->poFileBuffer = new FileStorage();
	assert(this->poFileBuffer);
}

ImageRGB::~ImageRGB()
{
	delete this->poRawImage;
	this->poRawImage = nullptr;

	delete this->poFileBuffer;
	this->poFileBuffer = nullptr;
}

lodepng::State &ImageRGB::GetState()
{
	return this->mState;
}

PixelRGBStorage &ImageRGB::GetPixelStorage()
{
	return *this->poRawImage;
}

FileStorage &ImageRGB::GetFileStorage()
{
	return *this->poFileBuffer;
}

PixelRGB *ImageRGB::GetPixelRGB()
{
	assert(this->poRawImage);

	this->pPixelRGB = (PixelRGB *)&(*this->poRawImage)[0];
	assert(this->pPixelRGB);

	return this->pPixelRGB;
}

unsigned int &ImageRGB::Width()
{
	return this->mWidth;
}

unsigned int &ImageRGB::Height()
{
	return this->mHeight;
}

unsigned int ImageRGB::GetPixelCount() const
{
	return this->mPixelCount;
}

void ImageRGB::SetPixelCount(unsigned int width, unsigned int height)
{
	this->mPixelCount = width * height;
}


void ImageRGB::LoadPngImage(const char *pFileName)
{
	assert(pFileName);
	unsigned int error;

	// Initialize state
	this->GetState().decoder.color_convert = 0;
	this->GetState().decoder.remember_unknown_chunks = 1;

	// Load file
	error = lodepng::load_file(this->GetFileStorage(), pFileName);
	assert(!error);

	// Decode file
	error = lodepng::decode(this->GetPixelStorage(),
							this->Width(),
							this->Height(),
							this->GetState(),
							this->GetFileStorage());
	assert(!error);

	this->SetPixelCount(this->Width(), this->Height());

	Trace::out("Read image: \n");
	Trace::out("	      File: %s\n", pFileName);
	Trace::out("	     Width: %d \n", this->Width());
	Trace::out("	    Height: %d \n", this->Height());
	Trace::out("\n");
}

void ImageRGB::WritePngImage(const char *pFileName)
{
	assert(pFileName);
	unsigned int error;

	this->GetFileStorage().erase(std::begin(this->GetFileStorage()), std::end(this->GetFileStorage()));

	error = lodepng_encode_file(pFileName,
								(const unsigned char *)this->GetPixelRGB(),
								this->Width(),
								this->Height(),
								LodePNGColorType::LCT_RGB,
								8);
	assert(!error);

	Trace::out("Write image: \n");
	Trace::out("	      File: %s\n", pFileName);
	Trace::out("	     Width: %d \n", this->Width());
	Trace::out("	    Height: %d \n", this->Height());
	Trace::out("\n");
}

// --- End of File ---
