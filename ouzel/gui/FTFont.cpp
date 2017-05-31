#include "FTFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "core/Engine.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

ouzel::FTFont::FTFont() : BMFont()
{
}

ouzel::FTFont::FTFont(const std::string & filename, int pt):
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
	
	FT_Error er = FT_New_Face(library,f.c_str(), 0, &face);

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

	//@TODO Get more than ASCII Characters 32 to 128
	for (unsigned char c = 32; c  < 128; c++)
	{
		//@TODO Use charmpas

		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			Log(Log::Level::ERR) << "Couldn't load char " << c;
		}

		Log(Log::Level::INFO) << "Loaded char: " << c;



		CharDescriptor nd;
		nd.height = face->glyph->metrics.height;
		

		//Get glyph metrics

		//Get Kerning Data

		//Get Bitmap


		
	}


}
