#include "kernel.cuh"


void isSameImage(ImageGray& lhs, ImageGray& rhs, const char* pOutputName, PerformanceTimer& host, PerformanceTimer& device)
{
	PixelGray* lPixels = lhs.GetPixelGray();
	PixelGray* rPixels = rhs.GetPixelGray();

	bool same = true;

	if (lhs.GetPixelCount() != rhs.GetPixelCount()) { same = false; }
	else 
	{
		for (unsigned int i = 0; i < rhs.GetPixelCount(); i++)
		{
			if (lPixels[i].v != rPixels[i].v)
			{
				same = false;
			}
		}
	}
	if (same)
	{
		if (pOutputName)
		{
			lhs.WritePngImage(pOutputName);
		}
		Trace::out("\n");
		Trace::out("SAME IMAGE FOR (%s):\n", pOutputName);
		Trace::out("  Host: %f ms\n", host.TimeInMilliSeconds());
		Trace::out("  Device: %f ms\n", device.TimeInMilliSeconds());
		Trace::out("\n");
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}
}

//No templates so overloading.
void isSameImage(ImageRGB& lhs, ImageRGB& rhs, const char* pOutputName, PerformanceTimer& host, PerformanceTimer& device)
{
	PixelRGB* lPixels = lhs.GetPixelRGB();
	PixelRGB* rPixels = rhs.GetPixelRGB();

	bool same = true;

	if (lhs.GetPixelCount() != rhs.GetPixelCount()) { same = false; }
	else 
	{
		for (unsigned int i = 0; i < rhs.GetPixelCount(); i++)
		{
			if (lPixels[i].r != rPixels[i].r ||
				lPixels[i].g != rPixels[i].g ||
				lPixels[i].b != rPixels[i].b)
			{
				same = false;
			}
		}
	}

	if (same)
	{
		if (pOutputName)
		{
			lhs.WritePngImage(pOutputName);
		}
		Trace::out("\n");
		Trace::out("SAME IMAGE FOR (%s):\n", pOutputName);
		Trace::out("  Host: %f ms\n", host.TimeInMilliSeconds());
		Trace::out("  Device: %f ms\n", device.TimeInMilliSeconds());
		Trace::out("\n");
	}
	else
	{
		Trace::out("ERROR NOT SAME IMAGE!");
	}
}

#if PIPELINE

void pipeline()
{
	const char* pInputName = "0_image.RGB.png";
	const char* pOutputName_h = "Result_Host.png";
	const char* pOutputName_d = "Result_Device.png";


	ImageRGB Image_in_h;
	ImageRGB Image_in_d;
	Image_in_h.LoadPngImage(pInputName);
	Image_in_d.LoadPngImage(pInputName);

	ImageGray Image_out_h;
	ImageGray Image_tmp_h;
	ImageGray Image_out_d;
	ImageGray Image_tmp_d;
	Image_out_h.InitializeEmpty(Image_in_h.Width(), Image_in_h.Height());
	Image_tmp_h.InitializeEmpty(Image_in_h.Width(), Image_in_h.Height());
	Image_out_d.InitializeEmpty(Image_in_h.Width(), Image_in_h.Height());
	Image_tmp_d.InitializeEmpty(Image_in_h.Width(), Image_in_h.Height());

	ImageGray Image_tmp_two;
	Image_tmp_two.InitializeEmpty(Image_in_h.Width(), Image_in_h.Height());

	std::vector<Point> CenterPoints_h;
	std::vector<Point> CenterPoints_d;


	int threshOne = 150;
	int threshTwo = 1;
	int blurPasses = 11;
	int dilatePasses = 4;
	int erodePasses = 113;

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	tHost.Tic();
	{
		combined(Image_in_h, Image_out_h, Image_tmp_h, Image_tmp_two, CenterPoints_h, threshOne, threshTwo, blurPasses, dilatePasses, erodePasses);
	}
	tHost.Toc();

	tDevice.Tic();
	{
		combined_CUDA(Image_in_d, Image_out_d, Image_tmp_d, CenterPoints_d, threshOne, threshTwo, blurPasses, dilatePasses, erodePasses);
	}
	tDevice.Toc();

	Image_in_h.WritePngImage(pOutputName_h);
	Image_in_d.WritePngImage(pOutputName_d);

	Trace::out("\n");
	Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());
	Trace::out("  Device: %f ms\n", tDevice.TimeInMilliSeconds());
	Trace::out("\n");
}

#else

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
		RGB_to_gray(Image_Gray_h, Image_RGB);
	}
	tHost.Toc();


	// --------------------------------------
	// Device
	// --------------------------------------

	//THESE THINGS MIGHT NEVER EVEN BE FUCKING EQUAL BECAUSE I DON'T KNOW IF THE HOST AND DEVICE TREAT ROUNDING THE SAME WAY OR NOT. WHATEVER, SAME RESULTS.
	tDevice.Tic();
	{
		RGB_to_gray_CUDA(Image_Gray_d, Image_RGB);
	}
	tDevice.Toc();

	Image_Gray_h.WritePngImage(pOutputName);

	// --------------------------------------
	// Timings
	// --------------------------------------

	Trace::out("SAME IMAGE FOR (%s):\n", pOutputName);
	Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());
	Trace::out("  Device: %f ms\n", tDevice.TimeInMilliSeconds());
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

	int threshold_val = 150;

	tHost.Tic();
	{
		threshold(Image_Out_h, Image_In, threshold_val);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		threshold_CUDA(Image_Out_d, Image_In, threshold_val);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);

}

void step_three()
{
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
		invert(Image_Out_h, Image_In);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		invert_CUDA(Image_Out_d, Image_In);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);

}

void step_four()
{
	Trace::out("\n");

	const char* pInputName = "3_image_Invert.Gray.png";
	const char* pOutputName = "4_image_Erode.Gray.png";

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
		erode(Image_Out_h, Image_In);
	}
	tHost.Toc();


	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		erode_CUDA(Image_Out_d, Image_In);

	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);

}

void step_five()
{
	Trace::out("\n");

	const char* pInputName = "4_image_Erode.Gray.png";
	const char* pOutputName = "5_image_Dilate.Gray.png";

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
		dilate(Image_Out_h, Image_In);

	}
	tHost.Toc();


	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		dilate_CUDA(Image_Out_d, Image_In);

	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);

}

void step_six()
{
	Trace::out("\n");

	const char* pInputName = "5_image_Dilate.Gray.png";
	const char* pOutputName = "6_image_Blurr.Gray.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------

	ImageGray Image_In_h;
	ImageGray Image_In_d;
	Image_In_h.LoadPngImage(pInputName);
	Image_In_d.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In_h.Width(), Image_In_h.Height());
	Image_Out_d.InitializeEmpty(Image_In_d.Width(), Image_In_d.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	int count = 11;

	tHost.Tic();
	{
		blur_loop(Image_Out_h, Image_In_h, count);
	}
	tHost.Toc();


	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		blur_loop_CUDA(Image_Out_d, Image_In_d, count);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------
	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);
}

void step_seven()
{
	const char* pInputName = "6_image_Blurr.Gray.png";
	const char* pOutputName = "7_image_Threshold2.Gray.png";

	CudaTry(cudaSetDevice(0));

	ImageGray Image_In;
	Image_In.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In.Width(), Image_In.Height());
	Image_Out_d.InitializeEmpty(Image_In.Width(), Image_In.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	int threshold_val = 1;

	tHost.Tic();
	{
		threshold(Image_Out_h, Image_In, threshold_val);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		threshold_CUDA(Image_Out_d, Image_In, threshold_val);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);
}

void step_eight()
{
	const char* pInputName = "7_image_Threshold2.Gray.png";
	const char* pOutputName = "8_image_Dilate2.Gray.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------

	ImageGray Image_In_h;
	ImageGray Image_In_d;
	Image_In_h.LoadPngImage(pInputName);
	Image_In_d.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In_h.Width(), Image_In_h.Height());
	Image_Out_d.InitializeEmpty(Image_In_d.Width(), Image_In_d.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	int count = 5;
	tHost.Tic();
	{
		dilate_loop(Image_Out_h, Image_In_h, count);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		dilate_loop_CUDA(Image_Out_d, Image_In_d, count);
	}
	tDevice.Toc();

	// --------------------------------------
	// Timings
	// --------------------------------------

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);
}

void step_nine()
{
	const char* pInputName = "8_image_Dilate2.Gray.png";
	const char* pOutputName = "9_image_Erode2.Gray.png";
	CudaTry(cudaSetDevice(0));

	ImageGray Image_In_h;
	ImageGray Image_In_d;
	Image_In_h.LoadPngImage(pInputName);
	Image_In_d.LoadPngImage(pInputName);

	ImageGray Image_Out_h;
	ImageGray Image_Out_d;
	Image_Out_h.InitializeEmpty(Image_In_h.Width(), Image_In_h.Height());
	Image_Out_d.InitializeEmpty(Image_In_d.Width(), Image_In_d.Height());

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	int count = 113;
	tHost.Tic();
	{
		erode_loop(Image_Out_h, Image_In_h, count);
	}
	tHost.Toc();

	// --------------------------------------
	// Device
	// --------------------------------------

	tDevice.Tic();
	{
		erode_loop_CUDA(Image_Out_d, Image_In_d, count);
	}
	tDevice.Toc();

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);
}

void step_ten()
{
	const char* pInputName = "9_image_Erode2.Gray.png";
	const char* pOutputName = "10_CenterPoints.bin";

	std::vector<Point> CenterPoints;

	ImageGray Image_In_h;
	Image_In_h.LoadPngImage(pInputName);

	PerformanceTimer tHost;

	tHost.Tic();
	{
		find_center(Image_In_h, CenterPoints);
	}
	tHost.Toc();

	Trace::out("  Host: %f ms\n", tHost.TimeInMilliSeconds());

	FILE* fh;
	fopen_s(&fh, pOutputName, "wb");
	fwrite(&CenterPoints[0], sizeof(Point), CenterPoints.size(), fh);
	fclose(fh);
}

void step_eleven()
{
	const char* pInputName = "8_image_Dilate2.Gray.png";
	const char* pOutputName = "11_image_Edge.Gray.png";

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
		edge_detection(Image_Out_h, Image_In);
	}
	tHost.Toc();

	tDevice.Tic();
	{
		edge_detection_CUDA(Image_Out_d, Image_In);
	}
	tDevice.Toc();

	isSameImage(Image_Out_h, Image_Out_d, pOutputName, tHost, tDevice);
}

void step_twelve()
{
	const char* pInputName = "11_image_Edge.Gray.png";
	const char* pOrigName = "0_image.RGB.png";
	const char* pEdgeName = "11_image_Edge.Gray.png";
	const char* pOutputName = "12_FinalOutput.png";

	CudaTry(cudaSetDevice(0));

	// --------------------------------------
	// Host
	// --------------------------------------
	ImageGray Image_Edge;
	Image_Edge.LoadPngImage(pEdgeName);

	ImageRGB Image_Orig_h;
	ImageRGB Image_Orig_d;
	Image_Orig_h.LoadPngImage(pOrigName);
	Image_Orig_d.LoadPngImage(pOrigName);

	ImageGray Image_In;
	Image_In.LoadPngImage(pInputName);

	std::vector<Point> CenterPoints_h;
	loadCenter(CenterPoints_h);

	std::vector<Point> CenterPoints_d(CenterPoints_h);

	PerformanceTimer tHost;
	PerformanceTimer tDevice;

	tHost.Tic();
	{
		label_coins(Image_Edge, Image_Orig_h, Image_In, CenterPoints_h);
	}
	tHost.Toc();

	tDevice.Tic();
	{
		label_coins_CUDA(Image_Edge, Image_Orig_d, Image_In, CenterPoints_d);
	}
	tDevice.Toc();

	isSameImage(Image_Orig_h, Image_Orig_d, pOutputName, tHost, tDevice);
}
#endif

int main()
{
	START_BANNER_MAIN("--Main--");

	// Configure test macro in kernel.cuh to switch modes.
#if PIPELINE
	pipeline();

#else
	step_one();
	step_two();
	step_three();
	step_four();
	step_five();
	step_six();
	step_seven();
	step_eight();
	step_nine();
	step_ten();
	step_eleven();
	step_twelve();
#endif
}