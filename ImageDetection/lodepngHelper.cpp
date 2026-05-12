// ---------------------------------------------------------------------------
//  LodePNG version 20260119
//  
//  Copyright (c) 2005-2026 Lode Vandevenne
//  
//  This software is provided 'as-is', without any express or implied
//  warranty. In no event will the authors be held liable for any damages
//  arising from the use of this software.
//  
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//  
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//  
//      2. Altered source versions must be plainly marked as such, and must not be
//      misrepresented as being the original software.
//  
//      3. This notice may not be removed or altered from any source
//      distribution.
// ---------------------------------------------------------------------------

#include "lodepngHelper.h"

/*
Display general info about the PNG.
*/
void displayPNGInfo(const LodePNGInfo &info)
{
	const LodePNGColorMode &color = info.color;

	Trace::out("\n");
	Trace::out("Compression method: %d \n", info.compression_method);
	Trace::out("     Filter method: %d \n", info.filter_method);
	Trace::out("  Interlace method: %d \n", info.interlace_method);
	Trace::out("        Color type: %d (LCT_RGB = 2) \n", color.colortype);
	Trace::out("         Bit depth: %d \n", color.bitdepth);
	Trace::out("    Bits per pixel: %d \n", lodepng_get_bpp(&color));
	Trace::out("Channels per pixel: %d \n", lodepng_get_channels(&color));
	Trace::out(" Is greyscale type: %d \n", lodepng_is_greyscale_type(&color));
	Trace::out("    Can have alpha: %d \n", lodepng_can_have_alpha(&color));
	Trace::out("      Palette size: %d \n", color.palettesize);
	Trace::out("     Has color key: %d \n", color.key_defined);

	if(color.key_defined)
	{
		Trace::out("Color key r: %d \n", color.key_r);
		Trace::out("Color key g: %d \n", color.key_g);
		Trace::out("Color key b: %d \n", color.key_b);
	}
	Trace::out("Texts: %d \n", info.text_num);
	for(size_t i = 0; i < info.text_num; i++)
	{
		Trace::out("Text: %d : %s \n", info.text_keys[i], info.text_strings[i]);
	}
	Trace::out("International texts: %d \n", info.itext_num);
	for(size_t i = 0; i < info.itext_num; i++)
	{
		std::cout << "Text: "
			<< info.itext_keys[i] << ", "
			<< info.itext_langtags[i] << ", "
			<< info.itext_transkeys[i] << ": "
			<< info.itext_strings[i] << std::endl << std::endl;
	}
	Trace::out("Time defined: %d \n", info.time_defined);
	if(info.time_defined)
	{
		const LodePNGTime &time = info.time;
		Trace::out("  year: %d\n", time.year);
		Trace::out(" month: %d\n", time.month);
		Trace::out("   day: %d\n", time.day);
		Trace::out("  hour: %d\n", time.hour);
		Trace::out("minute: %d\n", time.minute);
		Trace::out("second: %d\n", time.second);
	}
	Trace::out("Physical pixel dimensions and aspect ratio defined: %d\n", info.phys_defined);
	if(info.phys_defined)
	{
		Trace::out("physics X: %d \n", info.phys_x);
		Trace::out("physics Y: %d \n", info.phys_y);
		Trace::out("physics unit: %d (1 = meter) \n", info.phys_unit);
	}
}

/*
Display the names and sizes of all chunks in the PNG file.
*/
void displayChunkNames(const std::vector<unsigned char> &buffer)
{
	// Listing chunks is based on the original file, not the decoded png info.
	const unsigned char *chunk, *end;
	end = &buffer.back() + 1;
	chunk = &buffer.front() + 8;

	std::cout << std::endl << "Chunks:" << std::endl;
	std::cout << " type: length(s)";
	std::string last_type;
	while(chunk < end && end - chunk >= 8)
	{
		char type[5];
		lodepng_chunk_type(type, chunk);
		if(std::string(type).size() != 4)
		{
			std::cout << "this is probably not a PNG" << std::endl;
			return;
		}

		if(last_type != type)
		{
			std::cout << std::endl;
			std::cout << " " << type << ": ";
		}
		last_type = type;

		std::cout << lodepng_chunk_length(chunk) << ", ";

		chunk = lodepng_chunk_next_const(chunk, end);
	}
	std::cout << std::endl;
}

/*
Show the filtertypes of each scanline in this PNG image.
*/
void displayFilterTypes(const std::vector<unsigned char> &buffer, bool ignore_checksums)
{
	//Get color type and interlace type
	lodepng::State state;
	if(ignore_checksums)
	{
		state.decoder.ignore_crc = 1;
		state.decoder.zlibsettings.ignore_adler32 = 1;
	}
	unsigned w, h;
	unsigned error;
	error = lodepng_inspect(&w, &h, &state, &buffer[0], buffer.size());

	if(error)
	{
		std::cout << "inspect error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	if(state.info_png.interlace_method == 1)
	{
		std::cout << "showing filtertypes for interlaced PNG not supported by this example" << std::endl;
		return;
	}

	//Read literal data from all IDAT chunks
	const unsigned char *chunk, *begin, *end;
	end = &buffer.back() + 1;
	begin = chunk = &buffer.front() + 8;

	std::vector<unsigned char> zdata;

	while(chunk < end && end - chunk >= 8)
	{
		char type[5];
		lodepng_chunk_type(type, chunk);
		if(std::string(type).size() != 4)
		{
			std::cout << "this is probably not a PNG" << std::endl;
			return;
		}

		if(std::string(type) == "IDAT")
		{
			const unsigned char *cdata = lodepng_chunk_data_const(chunk);
			unsigned clength = lodepng_chunk_length(chunk);
			if(chunk + clength + 12 > end || clength > buffer.size() || chunk + clength + 12 < begin)
			{
				std::cout << "invalid chunk length" << std::endl;
				return;
			}

			for(unsigned i = 0; i < clength; i++)
			{
				zdata.push_back(cdata[i]);
			}
		}

		chunk = lodepng_chunk_next_const(chunk, end);
	}

	//Decompress all IDAT data
	std::vector<unsigned char> data;
	error = lodepng::decompress(data, &zdata[0], zdata.size());

	if(error)
	{
		std::cout << "decompress error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	//A line is 1 filter byte + all pixels
	size_t linebytes = 1 + lodepng_get_raw_size(w, 1, &state.info_png.color);

	if(linebytes == 0)
	{
		std::cout << "error: linebytes is 0" << std::endl;
		return;
	}

	std::cout << "Filter types: ";
	for(size_t i = 0; i < data.size(); i += linebytes)
	{
		std::cout << (int)(data[i]) << " ";
	}
	std::cout << std::endl;

}

// --- End of File ----
