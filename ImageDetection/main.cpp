#include "CudaFramework.h"
#include "ImageRGB.h"
#include "ImageGray.h"

#include "Host.h"


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

	ImageGray Image_Gray;
	Image_Gray.InitializeEmpty(Image_RGB.Width(), Image_RGB.Height());

	PerformanceTimer tHost;

	tHost.Tic();
	{
		Host_ImageProcess(Image_Gray, Image_RGB);
	}
	tHost.Toc();

	Image_Gray.WritePngImage(pOutputName);

	// --------------------------------------
	// Device
	// --------------------------------------



	// --------------------------------------
	// Timings
	// --------------------------------------

	Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());


	Trace::out("\n");



}