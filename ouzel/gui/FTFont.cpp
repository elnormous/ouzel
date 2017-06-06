#include "FTFont.h"


#include <vector>
#include <cassert>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fstream>
#include "core/Engine.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
namespace ouzel {
	ouzel::FTFont::FTFont() : BMFont()
	{
	}

	ouzel::FTFont::FTFont(const std::string & filename, int pt) 	{
		FT_Library library;
		FT_Face	   face;

		std::string f = ouzel::sharedApplication->getFileSystem()->getPath(filename);

		if (FT_Init_FreeType(&library))
		{
			Log(Log::Level::ERR) << "Failed to init Freetype Font Library ";
		}
		Log(Log::Level::INFO) << "Initialized Freetype Font Library ";

		FT_Error er = FT_New_Face(library, f.c_str(), 0, &face);

		if (er == FT_Err_Unknown_File_Format)
		{
			Log(Log::Level::ERR) << "File " << f.c_str() << " could be read but the format is unsupported";
		}
		else if (er) {
			Log(Log::Level::ERR) << "File " << f.c_str() << " could not be read";
		}

		if (FT_Set_Pixel_Sizes(face, 0, pt))
		{
		
			Log(Log::Level::ERR) << "Could not set pixel size";
		}

		std::map<uint32_t,  std::pair<Size2, std::vector<uint8_t>> > glyph_to_bitmap_data;


		height = 0;
		width = 0;
		

		//@TODO Get more than ASCII Characters 32 to 128
		for (unsigned char c = 32; c < 127; c++)
		{
			//@TODO Use charmpas

			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Log(Log::Level::ERR) << "Couldn't load char " << c;
			}


			Log(Log::Level::INFO) << "Loaded char: " << c;


			FT_Bitmap bitmap = face->glyph->bitmap;

			CharDescriptor nd;
			nd.xAdvance = 0;
			nd.xOffset = 0;
			nd.yOffset = 0;
			nd.height = bitmap.rows;
 			nd.width = bitmap.width;

			//@TODO get kerning data
	
			std::vector<uint8_t> current_buffer((bitmap.rows * bitmap.width) );
			memcpy(current_buffer.data(), bitmap.buffer, (bitmap.rows * bitmap.width));

			//The bitmap uses integers the size uses floats
			glyph_to_bitmap_data.emplace(c, std::make_pair(Size2(bitmap.width  , bitmap.rows),current_buffer));

			chars.emplace(c, nd);

			height = height < bitmap.rows ? bitmap.rows : height;

			width += bitmap.width;




		}

		std::vector<std::vector<uint8_t> > scanlines(height,std::vector<uint8_t>());

		int x = 0;

		//Width Row
		//For each char
		for (const auto &c : glyph_to_bitmap_data)
		{
			int char_height = c.second.first.height();
			int char_width = c.second.first.width();

			//Get dimensions in pixels
			chars.at(c.first).x = x;
			chars.at(c.first).y = 0;

			x += char_width;
			 
			//debug information
			char fs = static_cast<char>(c.first);
			std::string cs;
			cs += fs;

			//Calculate the number of extra rows
			int extra_rows = height - char_height;
			//Print extra row num
			Log(Log::Level::INFO) << "extra rows for: " << cs << ": " << extra_rows;
			//Get the amount of pixels for extra extra rows
			size_t extra_space_size = extra_rows * char_width;
			//make a new buffer for a char that is extra space size long
			std::vector<uint8_t> new_char_buffer(extra_space_size + char_height * char_width, 0x00);
			//Copy the buffer from the glyph to the buffer
			memcpy(new_char_buffer.data(), c.second.second.data(), char_height * char_width);
			assert(new_char_buffer.size() == height * char_width);
			//for each scanline
			for (int i = 0; i < height; i++)
			{
				size_t scanlines_pre_size = scanlines[i].size();
				scanlines[i].resize(scanlines_pre_size + char_width);
				memcpy(scanlines[i].data() + scanlines_pre_size, new_char_buffer.data() + (int)char_width * i, char_width);
			}
			
		}

		std::vector<uint8_t> buffer(scanlines[0].size() * height);
		for (int i = 0; i < height; i++)
		{
			memcpy(buffer.data() + scanlines[0].size()  * i, scanlines[i].data(), scanlines[0].size());
			
		}




		std::vector<uint8_t> buffertwo;
		std::vector<uint32_t> testBuffer(buffer.size());
		for (int i = 0; i < buffer.size(); i++)
		{
			testBuffer[i] = buffer[i] << 24 | buffer[i] << 16 | buffer[i] << 8 | buffer[i];
		}

		buffertwo.resize(buffer.size() * 4);
		memcpy(buffertwo.data(), testBuffer.data(),testBuffer.size() * 4);






		std::string name_and_pt = filename + toString(pt);

		sharedEngine->getCache()->getTextureFromData(name_and_pt, buffertwo, Size2(width, height));
		texture = name_and_pt;

		Log(Log::Level::INFO) << "Width: " << width << "Height: " << height;

		Log(Log::Level::INFO) << "Outed file";
		
		pages = 1;
		//The pt is the max height that the font can be
		lineHeight = pt;


		kern = std::map<std::pair<uint32_t, uint32_t>, int16_t>();
		kernCount = 0;

		base = 0;

	}
}
