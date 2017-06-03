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
#include "../LodePng.h"
namespace ouzel {
	ouzel::FTFont::FTFont() : BMFont()
	{
	}

	ouzel::FTFont::FTFont(const std::string & filename, int pt) :
		BMFont("arial.fnt")
	{
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

		auto size = ouzel::sharedEngine->getRenderer()->getSize();

		if (FT_Set_Pixel_Sizes(face, 0, pt))
		{
			Log(Log::Level::ERR) << "Could not set pixel size";
		}

		
		std::unordered_map<uint32_t,  std::pair<Size2, std::vector<uint8_t>> > glyph_to_bitmap_data;


		uint32_t min_height = 0;
		uint32_t width = 0;
		

		//@TODO Get more than ASCII Characters 32 to 128
		for (unsigned char c = 32; c < 128; c++)
		{
			//@TODO Use charmpas

			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				Log(Log::Level::ERR) << "Couldn't load char " << c;
			}

			Log(Log::Level::INFO) << "Loaded char: " << c;



			CharDescriptor nd;
			nd.height = face->glyph->metrics.height;
			nd.width = face->glyph->metrics.width;
			nd.xAdvance = face->glyph->metrics.horiAdvance;
			nd.yOffset = face->glyph->metrics.height - face->glyph->metrics.horiBearingY;
			nd.xOffset = face->glyph->metrics.width - face->glyph->metrics.horiBearingX;



			//@TODO get kerning data


			//Add in padding 
			auto bitmap = face->glyph->bitmap;

			//size_t preSize = buffer.size();
			//size_t newBufferSize = bitmap.rows * bitmap.width;

			//buffer.resize(buffer.size() + newBufferSize);
			//memcpy(buffer.data() + preSize, bitmap.buffer, newBufferSize);
			
			std::vector<uint8_t> current_buffer((bitmap.rows * bitmap.width) );

			memcpy(current_buffer.data(), bitmap.buffer, (bitmap.rows * bitmap.width));

			//The bitmap uses integers the size uses floats


			glyph_to_bitmap_data.emplace(c, std::make_pair(Size2(bitmap.width, bitmap.rows),current_buffer));

			chars.emplace(c, nd);

			min_height = min_height < bitmap.rows ? bitmap.rows : min_height;

			width += bitmap.width;




		}

		std::vector<std::vector<uint8_t> > scanlines(min_height,std::vector<uint8_t>());

		int x = 0;

		//Width Row
		//For each char
		for (const auto &c : glyph_to_bitmap_data)
		{
			chars.at(c.first).x = x;
			chars.at(c.first).y = 0;
			 
			x += c.second.first.width();

			int extra_rows = min_height - c.second.first.height();
			Log(Log::Level::INFO) << "Extra rows: " << extra_rows;
			size_t extra_space_size = extra_rows * c.second.first.width();
			std::vector<uint8_t> new_char_buffer(extra_space_size, 0x00);
			new_char_buffer.resize(new_char_buffer.size() + c.second.first.height() * c.second.first.width());
			memcpy(new_char_buffer.data() + extra_space_size, c.second.second.data(), c.second.first.height() * c.second.first.width());
			assert(new_char_buffer.size() == min_height * c.second.first.width());
			//for each scanline
			for (int i = 0; i < min_height; i++)
			{
				size_t scanlines_pre_size = scanlines[i].size();
				scanlines[i].resize(scanlines_pre_size + c.second.first.width());
				memcpy(scanlines[i].data() + scanlines_pre_size, new_char_buffer.data() + (int)c.second.first.width() * i, c.second.first.width());
			}
			
		}

		std::vector<uint8_t> buffer(scanlines[0].size() * min_height);
		for (int i = 0; i < min_height; i++)
		{
			memcpy(buffer.data() + scanlines[0].size()  * i, scanlines[i].data(), scanlines[0].size());
			
		}

		std::string name_and_pt = filename + toString(pt);

		sharedEngine->getCache()->getTextureFromData(name_and_pt, buffer, Size2());
		texture = name_and_pt;

		std::vector<unsigned char> png;
		auto error = lodepng::encode(png, buffer, width, min_height, LCT_GREY);
		if (error)
		{
			Log(Log::Level::ERR) << "Failed to encode file: " << lodepng_error_text(error);
		}

		if (lodepng::save_file(png, "outpng.png"))
		{
			Log(Log::Level::ERR) << "Failed to save file";
		}

		Log(Log::Level::INFO) << "Width: " << width << "Height: " << min_height;

		Log(Log::Level::INFO) << "Outed file";
		


	}
}
