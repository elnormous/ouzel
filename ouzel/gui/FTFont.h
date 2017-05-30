#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include "BMFont.h"

namespace ouzel
{

	class FTFont 
	{
	public:
		FTFont(std::string& filename, int pt);
	};
}