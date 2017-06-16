#include "FTFont.h"
#include <ft2build.h>
#include FT_FREETYPE_H




#include <vector>
#include <cassert>
#include <fstream>
#include "core/Engine.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"



		namespace ouzel {
			ouzel::FTFont::FTFont() : BMFont()
			{
			}

			ouzel::FTFont::FTFont(const std::string & filename, int pt, UTFChars flag) {
				if (!parseFont(filename, pt, flag))
				{
					Log(Log::Level::ERR) << "Failed to parse font " << filename;
				}

				kernCount = static_cast<uint16_t>(kern.size());

			}

			bool FTFont::parseFont(const std::string & filename, int pt, UTFChars flag)
			{
				FT_Library library;
				FT_Face	   face;
				std::string f = ouzel::sharedApplication->getFileSystem()->getPath(filename);

				if (FT_Init_FreeType(&library))
				{
					Log(Log::Level::ERR) << "Failed to init Freetype Font Library ";
					return false;
				}


				FT_Error er = FT_New_Face(library, f.c_str(), 0, &face);

				if (er == FT_Err_Unknown_File_Format)
				{
					Log(Log::Level::ERR) << "File " << f.c_str() << " could be read but the format is unsupported";
					return false;
				}
				else if (er)
				{
					Log(Log::Level::ERR) << "File " << f.c_str() << " could not be read";
					return false;
				}


				if (FT_Set_Pixel_Sizes(face, 0, pt))
				{
					Log(Log::Level::ERR) << "Could not set pixel size";
					return false;
				}

				std::map<uint32_t, std::pair<Size2, std::vector<uint8_t>> > glyph_to_bitmap_data;
				height = 0;
				width = 0;
				kern = std::map<std::pair<uint32_t, uint32_t>, int16_t>();
				std::vector<uint16_t> glyphs;

				if (flag && UTFChars::ASCII)
				{
					for (int i = 32; i < 127; i++)
					{
						glyphs.push_back(i);
					}
				}
				if (flag && UTFChars::ASCIIPLUS)
				{
					for (int i = 128; i < 256; i++)
					{
						glyphs.push_back(i);
					}
				}
				
				for (uint16_t c : glyphs)
				{
					if (FT_Load_Char(face, c, FT_LOAD_RENDER))
					{
						Log(Log::Level::ERR) << "Couldn't load char " << c;
					}
					FT_Vector v;
					for (uint16_t j : glyphs)
					{
						FT_Get_Kerning(face, j, c, FT_KERNING_DEFAULT, &v);
						if (v.x == 0) continue;
						kern.emplace(std::pair<uint32_t, uint32_t>(j, c), v.x >> 6);
					}
					FT_Bitmap bitmap = face->glyph->bitmap;
					CharDescriptor nd;
					nd.xAdvance = face->glyph->metrics.horiAdvance>>6;
					nd.height = bitmap.rows;
					nd.width = bitmap.width;
					nd.xOffset = face->glyph->metrics.horiBearingX>>6;
					nd.yOffset = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY)>>6;

					std::vector<uint8_t> current_buffer((bitmap.rows * bitmap.width));
					memcpy(current_buffer.data(), bitmap.buffer, (bitmap.rows * bitmap.width));

					glyph_to_bitmap_data.emplace(c, std::make_pair(Size2(bitmap.width, bitmap.rows), current_buffer));
					chars.emplace(c, nd);
					height = height < bitmap.rows ? bitmap.rows : height;
					width += bitmap.width;
				}
				std::vector<std::vector<uint8_t> > scanlines(height, std::vector<uint8_t>());
				int x = 0;
				for (const auto &c : glyph_to_bitmap_data)
				{
					int char_height = c.second.first.height();
					int char_width = c.second.first.width();
					chars.at(c.first).x = x;
					chars.at(c.first).y = 0;
					x += char_width;


					int extra_rows = height - char_height;
					chars.at(c.first).yOffset += extra_rows;
					size_t extra_space_size = extra_rows * char_width;
					std::vector<uint8_t> new_char_buffer(extra_space_size + char_height * char_width, 0x00);
					memcpy(new_char_buffer.data(), c.second.second.data(), char_height * char_width);
					assert(new_char_buffer.size() == height * char_width);
					for (int i = 0; i < height; i++)
					{
						size_t scanlines_pre_size = scanlines[i].size();
						scanlines[i].resize(scanlines_pre_size + char_width);
						memcpy(scanlines[i].data() + scanlines_pre_size, new_char_buffer.data() + (int)char_width * i, char_width);
					}

				}

				std::vector<uint32_t> b1(scanlines[0].size() * height);
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < scanlines[0].size(); j++)
					{
						uint8_t b = scanlines[i][j];
						b1[i * scanlines[0].size() + j] = b << 24 | b << 16 | b << 8 | b;
					}

				}

				std::vector<uint8_t> b2(b1.size() * 4);
				memcpy(b2.data(), b1.data(), b1.size() * 4);
				std::string name_and_pt = filename + toString(pt);
				sharedEngine->getCache()->getTextureFromData(name_and_pt, b2, Size2(width, height));
				texture = name_and_pt;
				pages = 1;
				lineHeight = pt;
				kernCount = kern.size();
				base = 0;
				return false;
			}
		} 