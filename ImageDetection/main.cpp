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


void step_one()
{
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
		Host_ImageProcess_one(Image_Gray_h, Image_RGB);
	}
	tHost.Toc();


	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		Device_ImageProcess_one(Image_Gray_d, Image_RGB);
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
		Image_Gray_h.WritePngImage(pOutputName);
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}

}

void step_two()
{
	const char* pInputName = "1_image.Gray.png";
	const char* pOutputName = "2_image_Threshold.Gray.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------

	ImageGray Image_In;
	Image_In.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In.Width(), Image_In.Height());
	Image_Out_d.InitializeEmpty(Image_In.Width(), Image_In.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;


	tHost.Tic();
	{
		Host_ImageProcess_two(Image_Out_h, Image_In, 150);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		Device_ImageProcess_two(Image_Out_d, Image_In, 150);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	if (isSameImage(Image_Out_h, Image_Out_d))
	{
		Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());
		Trace::out("  Device: %f ms\n", tDevice.TimeInMilliSeconds());
		Trace::out("\n");
		Image_Out_h.WritePngImage(pOutputName);
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}
}

void step_three()
{
	START_BANNER_MAIN("--Main--");
	Trace::out("\n");

	const char* pInputName = "2_image_Threshold.Gray.png";
	const char* pOutputName = "3_image_Invert.Gray.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------

	ImageGray Image_In;
	Image_In.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In.Width(), Image_In.Height());
	Image_Out_d.InitializeEmpty(Image_In.Width(), Image_In.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	tHost.Tic();
	{
		Host_ImageProcess_three(Image_Out_h, Image_In);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		Device_ImageProcess_three(Image_Out_d, Image_In);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	if (isSameImage(Image_Out_h, Image_Out_d))
	{
		Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());
		Trace::out("  Device: %f ms\n", tDevice.TimeInMilliSeconds());
		Trace::out("\n");
		Image_Out_h.WritePngImage(pOutputName);
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}
}

int main()
{
	START_BANNER_MAIN("--Main--");
	//step_one();
	//step_two();
	step_three();

}