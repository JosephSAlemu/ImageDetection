#include "CudaFramework.h"
#include "ImageRGB.h"
#include "ImageGray.h"
#include "kernel.cuh"
#include "Host.h"


bool isSameImage(ImageGray& lhs, ImageGray& rhs)
{
	PixelGray* lPixels = lhs.GetPixelGray();
	PixelGray* rPixels = rhs.GetPixelGray();

	if (lhs.GetPixelCount() != rhs.GetPixelCount()) { return false; }
	for (int i = 0; i < rhs.GetPixelCount(); i++)
	{
		if (!(lPixels[i].v == rPixels[i].v))
		{
			return false;
		}
	}
	return true;
}

int main()
{
	START_BANNER_MAIN("--Main--");
	Trace::out("\n");

	const char* pInputName = "0_image.RGB.png";
	const char* pOutputName = "1_image.Gray.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------

	ImageRGB Image_RGB;
	Image_RGB.LoadPngImage(pInputName);

	ImageGray Image_Gray_h;
	ImageGray Image_Gray_d;

	Image_Gray_h.InitializeEmpty(Image_RGB.Width(), Image_RGB.Height());
	Image_Gray_d.InitializeEmpty(Image_RGB.Width(), Image_RGB.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;


	tHost.Tic();
	{
		Host_ImageProcess(Image_Gray_h, Image_RGB);
	}
	tHost.Toc();

	//Image_Gray.WritePngImage(pOutputName);

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		Device_ImageProcess(Image_Gray_d, Image_RGB);
	}
	tDevice.Toc();



	// --------------------------------------
	// Timings
	// --------------------------------------
	if (isSameImage(Image_Gray_h, Image_Gray_d))
	{
		Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());
		Trace::out("  Device: %f ms\n", tDevice.TimeInMilliSeconds());
		Trace::out("\n");
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}




}